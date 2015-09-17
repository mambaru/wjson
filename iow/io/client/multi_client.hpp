#pragma once

#include <iow/logger/logger.hpp>
#include <mutex>
#include <memory>
#include <map>

namespace iow{ namespace io{ namespace client{

template<typename Client>
class multi_client
{
public:
  typedef Client client_type;
  typedef ::iow::io::io_id_t io_id_t;
  typedef ::iow::io::incoming_handler_t incoming_handler_t;
  typedef ::iow::io::outgoing_handler_t outgoing_handler_t;
  typedef std::shared_ptr<client_type> client_ptr;
  typedef std::map<io_id_t, client_ptr> client_map;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::recursive_mutex mutex_type;
  // TODO MUTEX!!!!!!!!!!! 
  
  multi_client(io_service_type& io)
    : _io_service(io)
  {}
  
  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    _create_and_start = [this, opt](io_id_t , incoming_handler_t handler) mutable -> client_ptr 
    {
      opt.connection.incoming_handler = [handler](data_ptr d, io_id_t id, outgoing_handler_t outgoing)
      {
        IOW_LOG_BEGIN( "multi_client incoming_handler begin [" << d << "]" )
        handler( std::move(d), id, outgoing );
        IOW_LOG_END( "multi_client incoming_handler end [" << d << "]" )
      };
      auto pconn = std::make_shared<client_type>(this->_io_service);
      IOW_LOG_DEBUG( "auto_client::_create_and_start -1-" )
      pconn->start(opt);
      IOW_LOG_DEBUG( "auto_client::_create_and_start -2-" )
      return pconn;
    };
  }

  template<typename Opt>
  void reconfigure(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    for ( auto& conn : _clients )
    {
      conn.second->reconfigure(std::forward<Opt>(opt));
    }
  }

  void stop()
  {
    IOW_LOG_DEBUG("multi_client stop lock ...")

    std::lock_guard<mutex_type> lk(_mutex);

    IOW_LOG_DEBUG("multi_client stop locked! stop clients")
    
    for ( auto& conn : _clients )
    {
      IOW_LOG_DEBUG("multi_client stop client...")
      conn.second->stop();
      IOW_LOG_DEBUG("multi_client stop client Done!")
    }
  }

  
  void send( data_ptr d, io_id_t io_id, outgoing_handler_t handler)
  {
    this->client(io_id, handler)->send( std::move(d) );
  }
  
  client_ptr client(io_id_t io_id, outgoing_handler_t handler)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    auto itr = _clients.find(io_id);
    if (itr != _clients.end()) 
    {
      return itr->second;
    }
    else
    {
      return this->set_handler_(io_id, std::move(handler));
    }
  }

  void set_handler(io_id_t io_id, outgoing_handler_t handler)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->set_handler_(io_id, std::move(handler));
  }

  void erase_handler(io_id_t io_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _clients.find(io_id);
    if ( itr != _clients.end() ) 
    {
      auto pconn = itr->second;
      pconn->shutdown([pconn](io_id_t){
        IOW_LOG_DEBUG("client connection shutdown READY")
      });
      _clients.erase(itr);
    }
  }

public:

  client_ptr set_handler_(io_id_t io_id, outgoing_handler_t&& handler)
  {
    auto pconn = _create_and_start(io_id, [handler](data_ptr d, io_id_t, outgoing_handler_t)
    {
      handler(std::move(d));
    });
    _clients[io_id] = pconn;
    return pconn;
  }

  io_service_type& _io_service;
  std::function<client_ptr(io_id_t, incoming_handler_t)> _create_and_start;
  client_map _clients;
  mutable mutex_type _mutex;
};

}}}
