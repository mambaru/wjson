#pragma once

#include <fas/aop.hpp>
#include <iow/mutex.hpp>
#include <mutex>


namespace iow{ namespace jsonrpc{

template<typename Handler>
class handler_map
{
public:
  typedef Handler handler_type;
  typedef std::shared_ptr<handler_type> handler_ptr;
  typedef typename handler_type::io_id_t io_id_t;

  // find or create 
  handler_ptr findocre(io_id_t io_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _handlers.find(io_id);
    if ( itr != _handlers.end() )
    {
      return itr->second;
    }
    auto handler = std::make_shared<handler_type>();
    _handlers[io_id] = handler;
    return handler;
  }

  bool erase(io_id_t io_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _handlers.find(io_id);
    if ( itr == _handlers.end() )
      return false;
    _handlers.erase(itr);
    return true;
    
  }

private:
  typedef std::map<io_id_t, handler_ptr> handler_map_t;
  typedef std::mutex mutex_type;

  handler_map_t _handlers;
  mutex_type _mutex;
};

}} // wfc

