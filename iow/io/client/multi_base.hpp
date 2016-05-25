#pragma once

#include <iow/logger/logger.hpp>
#include <mutex>
#include <memory>
#include <vector>

namespace iow{ namespace io{ namespace client{

template<typename Client>
class multi_base
{
public:
  typedef Client client_type;
  typedef typename client_type::data_ptr data_ptr;
  typedef std::shared_ptr<client_type> client_ptr;
  typedef std::vector<client_ptr> client_list;
  typedef typename client_type::io_service_type io_service_type;
  typedef std::mutex mutex_type;
  
  multi_base(io_service_type& io)
    : _current(0)
    , _io_service(io)
  {}
  
  template<typename Opt>
  void start(Opt opt, size_t count)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( !_clients.empty() )
      return;
    
    _clients.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
      _clients.push_back( std::make_shared<client_type>(_io_service) );
      _clients.back()->start( opt );
    }
  }

  template<typename Opt>
  void reconfigure(Opt opt, size_t count)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    while ( _clients.size() > count )
    {
      _clients.back()->stop();
      _clients.pop_back();
    }
    
    while ( _clients.size() < count )
    {
      _clients.push_back( std::make_shared<client_type>(_io_service) );
      _clients.back()->start( opt );
    }
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);

    for ( auto& conn : _clients )
    {
      conn->stop();
    }
    _clients.clear();
  }

  data_ptr send(data_ptr d)
  {
    client_ptr cli = nullptr;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      if ( _clients.empty() ) return std::move(d);
      if ( _current >= _clients.size() ) _current = 0;
      cli = _clients[ _current++ ];
    }
    return cli->send( std::move(d) );
  }
  
public:
  size_t _current;
  io_service_type& _io_service;
  client_list _clients;
  mutable mutex_type _mutex;
};

}}}
