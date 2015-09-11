#pragma once

#include <fas/aop.hpp>
#include <iow/io/descriptor/mtholder.hpp>
#include <iow/logger/logger.hpp>
#include <iow/io/client/connection.hpp>

#include <memory>
#include <cassert>
#include <boost/concept_check.hpp>

namespace iow{ namespace io{ namespace client{

/*
 * потребитель регистрирует с io_id_t и outgoing_handler_t
 * регистрация при старте (или автоматически при первом обращении)
 * для каждого потребителя свой коннект (несколько?) и пул потоков
 *
 */


class outgoing_map
{
public:
  typedef ::iow::io::io_id_t io_id_t;
  typedef ::iow::io::outgoing_handler_t outgoing_handler_t;
  typedef ::iow::io::data_ptr data_ptr;
  typedef std::mutex mutex_type;

  outgoing_map()
  {
    _iterator = _handlers.end();
  }

  void set( io_id_t io_id, outgoing_handler_t handler)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    assert(handler!=nullptr);
    _handlers[io_id]=handler;
  }

  bool has( io_id_t io_id )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _handlers.find(io_id)!=_handlers.end();
  }

  outgoing_handler_t get(io_id_t io_id) const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    auto itr = _handlers.find(io_id);
    if ( itr == _handlers.end() )
    {
      return nullptr;
    }
    return itr->second;
  }

  void erase( io_id_t io_id )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _handlers.erase(io_id);
    _iterator = _handlers.end();
  }

  data_ptr send(data_ptr d)
  {
    outgoing_handler_t handler;
    {
      std::lock_guard<mutex_type> lk(_mutex);

      if ( _handlers.empty() )
      {
        return std::move(d);
      }

      if ( _iterator == _handlers.end() )
      {
        _iterator = _handlers.begin();
      }

      handler = _iterator->second;
      ++_iterator;
    }

    handler( std::move(d) );
    return nullptr;
  }

private:

  typedef std::map< io_id_t, outgoing_handler_t> handler_map;
  typedef handler_map::const_iterator const_iterator;
  handler_map _handlers;
  const_iterator _iterator;
  mutable mutex_type _mutex;
};

/*
// На каждый объект несколько коннектов может быть
class producer
{
public:
  typedef outgoing_map::data_ptr data_ptr;
  typedef outgoing_map::io_id_t io_id_t;
  typedef outgoing_map::outgoing_handler_t outgoing_handler_t;
  typedef ::iow::io::incoming_handler_t incoming_handler_t;
  typedef std::vector< data_ptr > wait_data_t;
  typedef std::mutex mutex_type;

  producer(io_id_t io_id, incoming_handler_t handler, size_t outgoing_limit)
    : _source_id(io_id)
    , _source_handler(handler)
    , _outgoing_limit(outgoing_limit)
  {
    assert(_source_handler!=nullptr);
  }

  void reg(io_id_t io_id, outgoing_handler_t handler)
  {
    assert(handler!=nullptr);

    _remotes.set(io_id, std::move(handler));
    
    wait_data_t wd;

    {
      std::lock_guard<mutex_type> lk(_mutex);
      wd.swap(_wait_data);
    }
    
    auto itr = wd.begin();
    auto end = wd.end();
    for ( ; itr!=end; ++itr )
    {
      if ( auto d = _remotes.send(std::move(*itr) ) )
      {
        *itr = std::move(d);
        break;
      }
    }

    if ( itr != end )
    {
      std::lock_guard<mutex_type> lk(_mutex);
      std::move( itr, end, std::inserter(_wait_data, _wait_data.begin() ) );
    }
  }

  void unreg( io_id_t io_id )
  {
    _remotes.erase(io_id);
  }

  data_ptr send(data_ptr d)
  {
    if ( auto dd = _remotes.send(std::move(d) ) )
    {
      std::lock_guard<mutex_type> lk(_mutex);
      
      if ( _wait_data.size() < _outgoing_limit )
      {
        _wait_data.push_back( std::move(dd) );
      }
      else
      {
        IOW_LOG_ERROR("Drop message [" << d << "]")
        return std::move(dd);
      }
    }
    return nullptr;
  }

  void recv(data_ptr d, io_id_t io_id, outgoing_handler_t outgoing)
  {
    _source_handler(std::move(d), io_id, outgoing );
  }

private:
  
  io_id_t _source_id;
  incoming_handler_t _source_handler;
  size_t _outgoing_limit;
  outgoing_map _remotes;
  wait_data_t _wait_data;
  mutable mutex_type _mutex;
};
*/

template<typename Connection, typename A = fas::aspect<> >
class client 
  : public Connection
  , public std::enable_shared_from_this< client<Connection, A> >
{
public:
  typedef Connection super;
  typedef client<Connection> self;
  typedef typename super::descriptor_type descriptor_type;
  typedef ::iow::asio::io_service io_service_type;
  typedef typename super::mutex_type mutex_type;
  typedef typename super::outgoing_handler_type outgoing_handler_t;
  typedef boost::asio::deadline_timer reconnect_timer;
  typedef std::vector< data_ptr > wait_data_t;
  
  client( io_service_type& io)
    : super(std::move(descriptor_type(io)) )
    , _started(false)
    , _ready_for_write(false)
    , _reconnect_timeout_ms(0)
    , _connect_time_ms(0)
    , _reconnect_timer(io)
    , _wait_cursize(0)
    , _wait_maxsize(0)
    , _wait_wrnsize(0)
  {}
  
  client( io_service_type& io, descriptor_type&& desc)
    : super(std::move(desc) )
    , _started(false)
    , _ready_for_write(false)
    , _reconnect_timeout_ms(0)
    , _connect_time_ms(0)
    , _reconnect_timer(io)
    , _wait_cursize(0)
    , _wait_maxsize(0)
    , _wait_wrnsize(0)
  {}
  
  template<typename Opt>
  void start(Opt opt)
  {
    IOW_LOG_DEBUG( "Client start " << opt.addr << ":" << opt.port)
    std::lock_guard<mutex_type> lk( super::mutex() );
    if ( _started ) return;
    _started = true;
    _ready_for_write = false;
    _reconnect_timeout_ms = opt.reconnect_timeout_ms;
    _wait_maxsize = opt.wait_maxsize;
    _wait_wrnsize = opt.wait_wrnsize;

    this->initialize_(opt);
    super::connect_( *this, std::forward<Opt>(opt) );
  }

  template<typename Opt>
  void connect(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::connect_( *this, std::forward<Opt>(opt) );
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    if ( _started ) 
      return;
    _ready_for_write = false;
    super::stop_(*this);
  }

  void close()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    _ready_for_write = false;
    super::close_(*this);
  }

  void send(data_ptr d)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    
    if ( d==nullptr )
      return;

    IOW_LOG_DEBUG( "Client::send " << _ready_for_write << ", " << (_outgoing_handler!=nullptr) )
    if ( _ready_for_write && _outgoing_handler!=nullptr )
    {
      _outgoing_handler( std::move(d) );
    }
    else
    {
      if ( _wait_cursize >= _wait_maxsize )
      {
        IOW_LOG_ERROR("Client limit wait data size " << _wait_cursize << "( max: " << _wait_maxsize << ")" 
                      << " dropped: [" << d << "]")
      }
      else
      {
        if ( _wait_cursize >= _wait_wrnsize )
        {
          IOW_LOG_WARNING("Client limit wait data size " << _wait_cursize << "( max: " << _wait_maxsize << ")")
        }
        _wait_cursize += d->size();
        _wait_data.push_back( std::move(d) );
      }
    }
  }
  
private:
  
  template<typename Opt>
  void client_start_(Opt&& opt)
  {
    super::start_(*this, opt.connection);
  }
  
  void startup_handler_(io_id_t, outgoing_handler_t outgoing)
  {
    _ready_for_write = true;
    _outgoing_handler = outgoing;
    _wait_cursize = 0;
    IOW_LOG_DEBUG("startup_handler_" << (outgoing!=nullptr) )
    std::for_each(_wait_data.begin(), _wait_data.end(), [outgoing](data_ptr& d) 
    {
      IOW_LOG_DEBUG("startup_handler_" << d )
      outgoing(std::move(d));
    });
    _wait_data.clear();
  }
  
  template<typename H>
  void delayed_handler_(H handler)
  {
    time_t now = this->now_ms();
    time_t diff = now - this->_connect_time_ms;
    time_t reconnect_time = diff > this->_reconnect_timeout_ms ? 0 : this->_reconnect_timeout_ms - diff ;
    this->_reconnect_timer.expires_from_now( ::boost::posix_time::milliseconds( reconnect_time ) );
    this->_connect_time_ms = now;
    this->_reconnect_timer.async_wait( std::move(handler) );
  }
  
  template<typename OptPtr>
  void delayed_reconnect_(OptPtr popt)
  {
    std::weak_ptr<self> wthis = this->shared_from_this();
   
    this->delayed_handler_(
      [wthis, popt](const boost::system::error_code& ec) 
      {
        if ( auto pthis = wthis.lock() )
        {
          if ( ec == boost::asio::error::operation_aborted )
            return;
          // this->connect();
          pthis->connect(*popt);
        }
      } 
    );
  }

  template<typename Opt>
  void initialize_(Opt& opt)
  {
    auto popt = std::make_shared<Opt>(opt);
    std::weak_ptr<self> wthis = this->shared_from_this();
    
    auto startup_handler  = popt->connection.startup_handler;
    auto shutdown_handler = popt->connection.shutdown_handler;
    auto connect_handler  = popt->connect_handler;
    auto error_handler    = popt->error_handler;


    popt->connect_handler = [wthis, connect_handler, popt]()
    {
      if ( connect_handler ) connect_handler();
      
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->client_start_(*popt);
      }
    };
    
    popt->error_handler = [wthis, error_handler, popt](::iow::system::error_code ec)
    {
      if ( error_handler!=nullptr ) error_handler(ec);
      
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->delayed_reconnect_(popt);
      }
    };
    
    popt->connection.shutdown_handler 
      = [wthis, shutdown_handler, popt]( io_id_t io_id) 
    {
      if ( shutdown_handler ) shutdown_handler(io_id);
      
      if ( auto pthis = wthis.lock() )
      {
        pthis->stop();
        
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->delayed_reconnect_(popt);
      }
    };
    
    popt->connection.startup_handler 
      = [wthis, startup_handler]( io_id_t io_id, outgoing_handler_t outgoing)
    {
      IOW_LOG_DEBUG("client startup_handler " << (outgoing!=nullptr) )

      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->startup_handler_(io_id, outgoing);
      }

      if ( startup_handler != nullptr )
      {
        startup_handler( io_id, outgoing);
      }
    };
      
    if ( popt->connection.incoming_handler == nullptr )
    {
      popt->connection.incoming_handler
        = [wthis]( data_ptr d, io_id_t /*io_id*/, outgoing_handler_t /*outgoing*/)
      {
        IOW_LOG_ERROR("Client incoming_handler not set [" << d << "]" )
      }; 
    }

    opt = *popt;
  }


  time_t now_ms()
  {
    using namespace ::boost::posix_time;
    ptime time_t_epoch( ::boost::gregorian::date(1970,1,1)); 
    ptime now = microsec_clock::local_time();
    time_duration diff = now - time_t_epoch;
    return diff.total_milliseconds();
  }


private:
  bool _started;
  bool _ready_for_write;
  time_t _reconnect_timeout_ms;
  time_t _connect_time_ms;
  reconnect_timer _reconnect_timer;
  outgoing_handler_t _outgoing_handler;

  size_t      _wait_cursize;  
  size_t      _wait_maxsize;
  size_t      _wait_wrnsize;
  wait_data_t _wait_data;
};


template<typename Client >
class mtclient
  : public std::enable_shared_from_this< mtclient<Client> >
{
  typedef mtclient<Client> self;
  typedef ::iow::io::descriptor::mtholder<Client> clients_type;
public:

  typedef typename clients_type::io_service_type io_service_type;
  typedef std::mutex mutex_type;
  
  mtclient(io_service_type& io)
    : _clients(io)
  {
  }
  
  template<typename Opt>
  void start(Opt opt)
  {
    auto startup_handler = opt.connection.startup_handler;
    auto shutdown_handler = opt.connection.shutdown_handler;
    std::weak_ptr<self> wthis = this->shared_from_this();
    
    opt.connection.startup_handler = [wthis, startup_handler]( io_id_t io_id, outgoing_handler_t outgoing)
    {
      if (auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk(pthis->_mutex);
        pthis->_handlers.set(io_id, outgoing);
      }
      if ( startup_handler ) startup_handler(io_id, outgoing);
    };
    
    opt.connection.shutdown_handler 
      = [wthis, shutdown_handler]( io_id_t io_id) 
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk(pthis->_mutex);
        pthis->_handlers.erase(io_id);
      }
      if ( shutdown_handler ) shutdown_handler(io_id);
    };
    
    _clients.start( opt );
  }
  
  template<typename Opt>
  void reconfigure(Opt&& opt)
  {
    _clients.reconfigure( std::forward<Opt>(opt) );
  }
  
  void stop()
  {
    _clients.stop( );
  }
  
  data_ptr send(data_ptr d)
  {
    return _handlers.send( std::move(d) );
  }

private:
  
  clients_type _clients;
  outgoing_map _handlers;
  mutable mutex_type _mutex;
};

/*

template<typename Connection>
class mtconn
  : private ::iow::io::descriptor::mtdup<Connection>
  , public std::enable_shared_from_this< mtconn<Connection> >
{
  typedef ::iow::io::descriptor::mtdup<Connection> super;
  typedef mtconn<Connection> self;
  typedef boost::asio::deadline_timer reconnect_timer;

public:
  typedef std::shared_ptr<source> source_ptr;
  typedef source::io_id_t io_id_t;
  typedef source::outgoing_handler_t outgoing_handler_t;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::mutex_type mutex_type;
  typedef Connection connection_type;
  typedef typename connection_type::descriptor_type descriptor_type;

  mtconn(io_service_type& io, source_ptr source)
    : super( std::move( descriptor_type(io) ) )
    , _reconnect_timer(io)
    , _source(source)
    , _reconnect_timeout_ms(0)
    , _connect_time(0)
    , _started(false)
  {
  }
  
  template<typename Opt>
  void start(Opt opt)
  {
    _started = true;
    _reconnect_timeout_ms = opt.reconnect_timeout_ms;
    this->update_options_(opt);
    
    //super::origin()->connect(opt);
    this->reconnect_(opt);
  }

  void stop()
  {
    if ( _started )
    {
      _started = false;
      IOW_LOG_DEBUG("mtconn::stop")
      super::stop();
    }
  }
  
  void send(data_ptr d, outgoing_handler_t handler)
  {
    if ( auto dd = _source->send( std::move(d) ) )
    {
      if ( handler != nullptr )
      {
        handler( nullptr );
      }
    }
  }
  
private:
  
  template<typename Opt>
  void reconnect_(const Opt& opt)
  {
    super::origin()->connect(opt);
  }
  
  template<typename Opt>
  void start_connection_(Opt&& opt)
  {
    super::start(opt);
  }
  
  time_t now_ms()
  {
    using namespace ::boost::posix_time;
    ptime time_t_epoch( ::boost::gregorian::date(1970,1,1)); 
    ptime now = microsec_clock::local_time();
    time_duration diff = now - time_t_epoch;
    return diff.total_milliseconds();
  }
  
  template<typename Opt>
  void update_options_(Opt& opt_orig)
  {
    auto popt = std::make_shared<Opt>(opt_orig);
    
    std::weak_ptr<self> wthis = this->shared_from_this();
    auto startup_handler = popt->connection.startup_handler;
    auto shutdown_handler = popt->connection.shutdown_handler;
    auto connect_handler = popt->connect_handler;
    auto error_handler = popt->error_handler;
    
    popt->connect_handler = [wthis, connect_handler, popt](){
      if ( connect_handler ) connect_handler();
      if ( auto pthis = wthis.lock() )
      {
        pthis->start_connection_(*popt);
      }
    };

    popt->error_handler = [wthis, error_handler, popt](::iow::system::error_code ec)
    {
      
      if ( error_handler!=nullptr ) error_handler(ec);
      
      if ( auto pthis = wthis.lock() )
      {
        time_t now = pthis->now_ms();
        time_t diff = now - pthis->_connect_time;
        time_t reconnect_time = diff > pthis->_reconnect_timeout_ms ? 0 : pthis->_reconnect_timeout_ms - diff ;
        pthis->_reconnect_timer.expires_from_now( ::boost::posix_time::milliseconds( reconnect_time ) );
        pthis->_connect_time = now;
        
        
        pthis->_reconnect_timer.async_wait( [wthis, popt](const boost::system::error_code& ec) 
        {
          if ( auto pthis = wthis.lock() )
          {
            if ( ec == boost::asio::error::operation_aborted )
              return;
            // this->connect();
            pthis->reconnect_(*popt);
          }
        });
      }
    };

    popt->connection.startup_handler 
      = [wthis, startup_handler]( io_id_t io_id, outgoing_handler_t outgoing)
    {
      if ( auto pthis = wthis.lock() )
      {
        // std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->_source->reg(io_id, outgoing );
      }

      if ( startup_handler != nullptr )
      {
        startup_handler( io_id, outgoing);
      }
    };
      
    popt->connection.shutdown_handler 
      = [wthis, shutdown_handler, popt]( io_id_t io_id) 
    {
      IOW_LOG_MESSAGE("Client shutdown")
      if ( shutdown_handler ) shutdown_handler(io_id);
      if ( auto pthis = wthis.lock() )
      {
        pthis->stop();
        //std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->_source->unreg(io_id);
      }
      popt->error_handler( ::boost::system::error_code() );
    };

    if ( popt->connection.incoming_handler == nullptr )
    {
      popt->connection.incoming_handler
        = [wthis]( data_ptr d, io_id_t io_id, outgoing_handler_t outgoing)
      {
        if ( auto pthis = wthis.lock() )
        {
          pthis->_source->recv( std::move(d), io_id, outgoing );
        }
      }; 
    }
    
    opt_orig = *popt;
  }

  std::atomic<bool> _started;
  reconnect_timer _reconnect_timer;

  time_t _reconnect_timeout_ms;
  time_t _connect_time;

  source_ptr _source;
};


template<typename Connection>
class client
{
  typedef mtconn<Connection> mtconn_t;
  typedef std::shared_ptr<mtconn_t> mtconn_ptr;
  typedef typename mtconn_t::mutex_type mutex_type;

public:

  typedef ::iow::io::incoming_handler_t incoming_handler_t;
  typedef typename mtconn_t::io_service_type io_service_type;
  typedef typename mtconn_t::io_id_t io_id_t;
  typedef typename mtconn_t::outgoing_handler_t outgoing_handler_t;

  client(io_service_type& io)
    : _io_service(io)
  {}

  template<typename Opt>
  void start(Opt&& opt)
  {
    _create_and_start = [this, opt](io_id_t io_id, incoming_handler_t handler) -> mtconn_ptr
    {
      auto pconn = std::make_shared<mtconn_t>(
        this->_io_service,
        std::make_shared<source>(io_id, handler, 1000)
      );
      pconn->start(opt);
      return pconn;
    };
  }

  void stop()
  {
    for ( auto& conn : _connects )
    {
      conn.second->stop();
    }
  }

  // TODO: переименовать
  void reg(io_id_t io_id, incoming_handler_t handler)
  {
    this->reg_(io_id, handler);
  }

  void send( data_ptr d, io_id_t io_id, outgoing_handler_t handler)
  {
    mtconn_ptr pconn = nullptr;
    auto itr = _connects.find(io_id);
    if (itr != _connects.end()) 
    {
      pconn = itr->second;
    }
    else
    {
      pconn = this->reg_(io_id, [handler](data_ptr d, io_id_t, outgoing_handler_t)
      {
        handler(std::move(d));
      });
    }
    pconn->send( std::move(d), handler );
  }

private:

  mtconn_ptr reg_(io_id_t io_id, incoming_handler_t handler)
  {
    auto itr = _connects.find(io_id);
    if ( itr != _connects.end() )
    {
      return itr->second;
    }

    auto pconn = _create_and_start( io_id, handler);

    _connects.insert( std::make_pair(io_id, pconn) );

    return pconn;
  }

private:

  std::function<mtconn_ptr(io_id_t, incoming_handler_t)> _create_and_start;
  io_service_type& _io_service;

  typedef std::map<io_id_t, mtconn_ptr > connect_map;
  connect_map _connects;
};

*/

template<typename Client>
class auto_client
{
public:
  typedef Client client_type;
  typedef ::iow::io::io_id_t io_id_t;
  typedef ::iow::io::incoming_handler_t incoming_handler_t;
  typedef ::iow::io::outgoing_handler_t outgoing_handler_t;
  typedef std::shared_ptr<client_type> client_ptr;
  typedef std::map<io_id_t, client_ptr> client_map;
  typedef ::iow::asio::io_service io_service_type;
  
  // TODO MUTEX!!!!!!!!!!! 
  
  auto_client(io_service_type& io)
    : _io_service(io)
  {}
  
  template<typename Opt>
  void start(Opt opt)
  {
    _create_and_start = [this, opt](io_id_t io_id, incoming_handler_t handler) -> client_ptr
    {
      opt.connection.incoming_handler = handler;
      auto pconn = std::make_shared<client_type>(this->_io_service);
      pconn->start(opt);
      return pconn;
    };
  }

  template<typename Opt>
  void reconfigure(Opt&& opt)
  {
    for ( auto& conn : _clients )
    {
      conn.second->reconfigure();
    }
  }

  void stop()
  {
    for ( auto& conn : _clients )
    {
      conn.second->stop();
    }
  }

  
  void send( data_ptr d, io_id_t io_id, outgoing_handler_t handler)
  {
    auto itr = _clients.find(io_id);
    if (itr != _clients.end()) 
    {
      itr->second->send( std::move(d) );
    }
    else
    {
      auto pconn = _create_and_start(io_id, [handler](data_ptr d, io_id_t, outgoing_handler_t)
      {
        handler(std::move(d));
      });
      
      _clients[io_id] = pconn;
      pconn->send( std::move(d) );
    }
  }
  
public:
  io_service_type& _io_service;
  std::function<client_ptr(io_id_t, incoming_handler_t)> _create_and_start;
  client_map _clients;
};

}}}
