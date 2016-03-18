#pragma once

#include <fas/aop.hpp>
#include <iow/io/descriptor/mtholder.hpp>
#include <iow/logger/logger.hpp>
#include <iow/io/client/connection.hpp>

#include <memory>
#include <cassert>
#include <iow/asio.hpp>

namespace iow{ namespace io{ namespace client{

template<typename Connection, typename A = fas::aspect<> >
class client 
  : public Connection
  , public std::enable_shared_from_this< client<Connection, A> >
{
public:
  typedef Connection super;
  typedef client<Connection> self;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::descriptor_type descriptor_type;
  typedef ::iow::asio::io_service io_service_type;
  typedef typename super::mutex_type mutex_type;
  typedef typename super::outgoing_handler_type outgoing_handler_t;
  typedef ::iow::asio::deadline_timer reconnect_timer;
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
    IOW_LOG_DEBUG( "Client start " << opt.addr << ":" << opt.port << " opt.wait_maxsize =" << opt.wait_maxsize )
    std::lock_guard<mutex_type> lk( super::mutex() );
    if ( _started ) return;
    _started = true;
    _ready_for_write = false;
    _reconnect_timeout_ms = opt.reconnect_timeout_ms;
    _wait_maxsize = opt.wait_maxsize;
    _wait_wrnsize = opt.wait_wrnsize;

    this->initialize_(opt);
    super::connect_( *this, opt );
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
    if ( !_started ) 
      return;
    _started = false;
    _ready_for_write = false;
    super::stop_(*this);
  }

  template<typename Handler>
  void shutdown(Handler&& handler)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::shutdown_( *this, std::forward<Handler>(handler) );
  }

  void close()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    IOW_LOG_DEBUG( "client::close()..." )
    _ready_for_write = false;
    super::close_(*this);
    IOW_LOG_DEBUG( "client::close()... Done" )
  }

  data_ptr send(data_ptr d)
  {
    IOW_LOG_DEBUG( "client::send [" << d << "]" )
    
    std::lock_guard<mutex_type> lk( super::mutex() );
    
    IOW_LOG_DEBUG( "Client::send " << _ready_for_write << ", " << (_outgoing_handler!=nullptr) << ", d==nullptr(" << (d==nullptr) << ")" )
    if ( d==nullptr )
      return nullptr;
    
    if ( _ready_for_write && _outgoing_handler!=nullptr )
    {
      /*auto oh = _outgoing_handler;
      super::mutex().unlock();
      oh( std::move(d) );*/
      _outgoing_handler( std::move(d) );
      //super::mutex().lock();
    }
    else
    {
      IOW_LOG_DEBUG( "Client::send wait [" << d << "]")
      if ( _wait_cursize >= _wait_maxsize )
      {
        IOW_LOG_ERROR("Client limit wait data size " << _wait_cursize << "( max: " << _wait_maxsize << ")" 
                      << " dropped: [" << d << "]")
        return std::move(d);
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
    return nullptr;
  }
  
private:
  
  template<typename Opt>
  void client_start_(Opt&& opt)
  {
    super::start_(*this, opt.connection);
  }
  
  void startup_handler_(io_id_t, outgoing_handler_t outgoing)
  {
    IOW_LOG_DEBUG("client::startup_handler_ _wait_data.size()=" << _wait_data.size() )
    _ready_for_write = true;
    _outgoing_handler = outgoing;
    _wait_cursize = 0;
    std::for_each(_wait_data.begin(), _wait_data.end(), [outgoing](data_ptr& d) 
    {
      IOW_LOG_DEBUG("client startup push [" << d << "]" << (d!=nullptr) )
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
      [wthis, popt](const ::iow::system::error_code& ec) 
      {
        if ( auto pthis = wthis.lock() )
        {
          if ( ec == ::iow::asio::error::operation_aborted )
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
      IOW_LOG_DEBUG("client shutdown_handler -1-" )

      if ( shutdown_handler ) shutdown_handler(io_id);

      IOW_LOG_DEBUG("client shutdown_handler -2-" )
      
      if ( auto pthis = wthis.lock() )
      {
        IOW_LOG_DEBUG("client shutdown_handler -3-" )

        pthis->stop();
        IOW_LOG_DEBUG("client shutdown_handler -4-" )
        
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->delayed_reconnect_(popt);
        IOW_LOG_DEBUG("client shutdown_handler -5-" )

      }
    };
    
    popt->connection.startup_handler 
      = [wthis, startup_handler]( io_id_t io_id, outgoing_handler_t outgoing)
    {
      IOW_LOG_DEBUG("client startup_handler " << (outgoing!=nullptr) )

      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        IOW_LOG_DEBUG("===1===" )
        pthis->startup_handler_(io_id, outgoing);
        IOW_LOG_DEBUG("===2===" )
      }

      if ( startup_handler != nullptr )
      {
        IOW_LOG_DEBUG("---1---" )
        startup_handler( io_id, outgoing);
        IOW_LOG_DEBUG("---2---" )
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

}}}
