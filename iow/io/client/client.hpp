#pragma once

#include <iow/io/descriptor/mtdup.hpp>
#include <iow/logger/logger.hpp>
#include <../../cdaemon/geoip_converter/types.h>

#include <memory>
#include <cassert>

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

  outgoing_map()
  {
    _iterator = _handlers.end();
  }

  void set( io_id_t io_id, outgoing_handler_t handler)
  {
    assert(handler!=nullptr);
    
    _handlers[io_id]=handler;
  }
  
  bool has( io_id_t io_id )
  {
    return _handlers.find(io_id)!=_handlers.end();
  }

  outgoing_handler_t get(io_id_t io_id) const
  {
    auto itr = _handlers.find(io_id);
    if ( itr == _handlers.end() )
    {
      return nullptr;
    }
    
    return itr->second;
  }
  
  void erase( io_id_t io_id )
  {
    _handlers.erase(io_id);
  }

  data_ptr send(data_ptr d)
  {
    IOW_LOG_MESSAGE("outgoing_map::send -1-")
    if ( _handlers.empty() )
    {
      return std::move(d);
    }
    
    IOW_LOG_MESSAGE("outgoing_map::send -2-")
    if ( _iterator == _handlers.end() )
    {
      _iterator = _handlers.begin();
    }

    IOW_LOG_MESSAGE("outgoing_map::send -3- _iterator->second!=nullptr ?  "  << (_iterator->second!=nullptr) )
    _iterator->second( std::move(d) );
    IOW_LOG_MESSAGE("outgoing_map::send -4-")
    ++_iterator;
    
    return nullptr;
  }

private:

  typedef std::map< io_id_t, outgoing_handler_t> handler_map;
  typedef handler_map::const_iterator const_iterator;
  handler_map _handlers;
  const_iterator _iterator;
};


// На каждый объект несколько коннектов может быть
class source
{
public:
  typedef outgoing_map::data_ptr data_ptr;
  typedef outgoing_map::io_id_t io_id_t;
  typedef outgoing_map::outgoing_handler_t outgoing_handler_t;
  typedef ::iow::io::incoming_handler_t incoming_handler_t;
  typedef std::vector< data_ptr > wait_data_t;
  
  source(io_id_t io_id, incoming_handler_t handler, size_t wait_limit)
    : _source_id(io_id)
    , _source_handler(handler)
  {
    assert(_source_handler!=nullptr);
  }
  
  void reg(io_id_t io_id, outgoing_handler_t handler)
  {
    assert(handler!=nullptr);
    
    _remotes.set(io_id, std::move(handler));
    
    auto itr = _wait_data.begin();
    auto end = _wait_data.end();
    for ( ; itr!=end; ++itr )
    {
      if ( auto d = _remotes.send(std::move(*itr) ) )
      {
        *itr = std::move(d);
        break;
      }
    }
    
    _wait_data.erase( _wait_data.begin(), itr );
      
  }

  void unreg( io_id_t io_id )
  {
    _remotes.erase(io_id);
  }
  
  data_ptr send(data_ptr d)
  {
    IOW_LOG_MESSAGE("source::send -1-")
    if ( auto dd = _remotes.send(std::move(d) ) )
    {
      if ( _wait_data.size() < _wait_limit )
      {
        IOW_LOG_MESSAGE("source::send -1.1- push back size=" << _wait_data.size())
        _wait_data.push_back( std::move(dd) );
      }
      else
      {
        IOW_LOG_MESSAGE("source::send -1.1- push back FAIL size=" << _wait_data.size())
        return std::move(dd);
      }
    }
    IOW_LOG_MESSAGE("source::send -2-")
    return nullptr;
  }
  
  void recv(data_ptr d, io_id_t io_id, outgoing_handler_t outgoing)
  {
    _source_handler(std::move(d), io_id, outgoing );
    
    // Иногда падает на брокен пайп или подвисает и не убивается 
    std::string kuka = "Кукарача";
    outgoing( std::make_unique<data_type>(kuka.begin(), kuka.end() ) );
  }

private:
  
  io_id_t _source_id;
  incoming_handler_t _source_handler;
  size_t _wait_limit;
  outgoing_map _remotes;
  wait_data_t _wait_data;
};

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
  {
  }
  
  template<typename Opt>
  void start(Opt opt)
  {
    this->update_options_(opt);
    
    _reconnect_timeout_ms = opt.reconnect_timeout_ms;
    
    auto connect_handler = opt.connect_handler;
    auto error_handler = opt.error_handler;
    
    std::weak_ptr<self> wthis = this->shared_from_this();
    
    opt.connect_handler = [connect_handler, wthis, opt](){
      IOW_LOG_MESSAGE("Client CONNECTED!!! ????")
      if ( connect_handler ) connect_handler();
      if ( auto pthis = wthis.lock() )
      {
        pthis->start_connection_(opt);
      }
    };

    opt.error_handler = [error_handler](::iow::system::error_code ec){
      IOW_LOG_MESSAGE("Client CONNECT ERROR!!! " << ec.message())
      error_handler(ec);
    };

    super::origin()->connect(opt);
  }

  void stop()
  {
    super::stop();
  }
  
  void send(data_ptr d, outgoing_handler_t handler)
  {
    IOW_LOG_MESSAGE("mtconn::send -1-")
    if ( auto dd = _source->send( std::move(d) ) )
    {
      IOW_LOG_MESSAGE("mtconn::send -fail-")
      if ( handler != nullptr )
      {
        handler( nullptr );
      }
    }
    IOW_LOG_MESSAGE("mtconn::send -2-")
  }
  
private:
  
  template<typename Opt>
  void start_connection_(Opt&& opt)
  {
    super::start(opt);
  }
  
  template<typename Opt>
  void update_options_(Opt& opt)
  {
    std::weak_ptr<self> wthis = this->shared_from_this();
    auto startup_handler = opt.connection.startup_handler;
    
    
    opt.connection.startup_handler = super::origin()->wrap([wthis, startup_handler]( io_id_t io_id, outgoing_handler_t outgoing)
    {
      IOW_LOG_MESSAGE("Connected!!! -1-");
      assert(outgoing!=nullptr);
      if ( auto pthis = wthis.lock() )
      {
        IOW_LOG_MESSAGE("Connected!!! -2-");
        pthis->_source->reg(io_id, outgoing );
        IOW_LOG_MESSAGE("Connected!!! -3-");
      }

      if ( startup_handler != nullptr )
      {
        IOW_LOG_MESSAGE("Connected!!! -4-");
        startup_handler( io_id, outgoing);
        IOW_LOG_MESSAGE("Connected!!! -5-");
      }
    });
    
    if ( opt.connection.incoming_handler == nullptr )
    {
      opt.connection.incoming_handler = super::origin()->wrap([wthis]( data_ptr d, io_id_t io_id, outgoing_handler_t outgoing)
      {
        if ( auto pthis = wthis.lock() )
        {
          // TODO: убрать! 
          assert(outgoing!=nullptr);
          pthis->_source->recv( std::move(d), io_id, outgoing );
        }
      }); 
    }
  }

  reconnect_timer _reconnect_timer;

  time_t _reconnect_timeout_ms;

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
    // TODO:
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
    IOW_LOG_MESSAGE("client send -2-");
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

}}}
