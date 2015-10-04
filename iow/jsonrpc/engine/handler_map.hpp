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

  /// @param[in] reg_io Значение reinit при следующем вызове 
  /// @param[out] reinit Требуеться инициализация. Всегда true при первом вызове, при последующих принимает reg_io от передыдущего вызова
  handler_ptr findocre(io_id_t io_id, bool reg_io, bool& reinit)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _handlers.find(io_id);
    if ( itr != _handlers.end() )
    {
      reinit = itr->second.second;
      itr->second.second = reg_io;
      return itr->second.first;
    }
    reinit = true;
    auto handler = std::make_shared<handler_type>();
    _handlers[io_id] = std::make_pair(handler, reg_io);
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

  void clear()
  {
    handler_map_t tmp_map;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      tmp_map.swap(_handlers);
    }

    for (auto& tmp : tmp_map)
    {
      if (tmp.second.first != nullptr ) 
      {
        tmp.second.first->stop();
      }
    }
  }

private:
  typedef std::map<io_id_t, std::pair<handler_ptr, bool> > handler_map_t;
  typedef std::mutex mutex_type;

  handler_map_t _handlers;
  mutex_type _mutex;
};

}} // wfc

