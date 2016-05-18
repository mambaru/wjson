#pragma once

#include <iow/logger/logger.hpp>
#include <mutex>
#include <memory>
#include <vector>


// удалить
#include <thread>

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

template<typename Client>
class multi_client: public multi_base<Client>
{
  typedef multi_base<Client> super;

public:
  typedef typename super::io_service_type io_service_type;
  
  multi_client(io_service_type& io)
    : super(io)
  {}
  
  template<typename Opt>
  void start(Opt opt)
  {
    super::start(std::move(opt), opt.connect_count);
  }

  template<typename Opt>
  void reconfigure(Opt opt, size_t count)
  {
    super::reconfigure(std::move(opt), opt.connect_count);
  }
};

template<typename Client>
class multi_thread: public multi_base<Client>
{
  typedef multi_base<Client> super;

public:
  typedef typename super::io_service_type io_service_type;
  
  multi_thread(io_service_type& io)
    : super(io)
  {}
  
  template<typename Opt>
  void start(Opt opt)
  {
    super::start(std::move(opt), threads_(opt) );
  }

  template<typename Opt>
  void reconfigure(Opt opt, size_t count)
  {
    super::reconfigure(std::move(opt), threads_(opt) );
  }
private: 
  template<typename Opt>
  size_t threads_(Opt& opt) const 
  {
    return opt.threads==0 ? 1 : opt.threads;
  }
};

/*  
template<typename Client>
class multi_client
{
public:
  typedef Client client_type;
  typedef typename client_type::data_ptr data_ptr;
  typedef std::shared_ptr<client_type> client_ptr;
  typedef std::vector<client_ptr> client_list;
  typedef typename client_type::io_service_type io_service_type;
  typedef std::mutex mutex_type;
  
  multi_client(io_service_type& io)
    : _current(0)
    , _io_service(io)
  {}
  
  template<typename Opt>
  void start(Opt opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _clients.reserve(opt.connect_count);
    for (size_t i = 0; i < opt.connect_count; ++i)
    {
      _clients.push_back( std::make_shared<client_type>(_io_service) );
      _clients.back()->start( opt );
    }
  }

  template<typename Opt>
  void reconfigure(Opt opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    while ( _clients.size() > opt.connect_count )
    {
      _clients.back()->stop();
      _clients.pop_back();
    }
    
    while ( _clients.size() < opt.connect_count )
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
  int _current;
  io_service_type& _io_service;
  client_list _clients;
  mutable mutex_type _mutex;
};
*/
}}}
