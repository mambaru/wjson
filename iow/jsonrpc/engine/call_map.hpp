#pragma once

#include <iow/mutex.hpp>
#include <iow/jsonrpc/incoming/incoming_holder.hpp>

#include <queue>
#include <mutex>
#include <chrono>


namespace iow{ namespace jsonrpc{

class call_map
{
public:
  typedef std::function<void(incoming_holder holder)> result_handler_t;
  typedef outgoing_holder::call_id_t call_id_t; 
  typedef std::map<call_id_t, result_handler_t> result_map;
  typedef ::iow::rwlock<std::mutex> mutex_type;

  typedef std::pair<time_t, call_id_t> time_pair;
  typedef std::priority_queue< time_pair > time_queue;
  typedef std::deque<call_id_t> call_list;
  
  void set_lifetime(time_t lifetime_ms) 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _lifetime_ms = lifetime_ms;
    if ( _lifetime_ms == 0 )
    {
      time_queue().swap(_time_queue);
    }
  }
  

  void set(call_id_t call_id, result_handler_t result)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _result_map[call_id] = result;
    if ( _lifetime_ms != 0 )
    {
      _time_queue.emplace( this->now_ms() + _lifetime_ms, call_id);
    }
  }
  
  result_handler_t detach(call_id_t call_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    result_handler_t result = nullptr;
    auto itr = _result_map.find(call_id);
    if ( itr!=_result_map.end() )
    {
      result = std::move( itr->second );
      _result_map.erase(itr);
    }
    return std::move(result);
  }
  
  void clear()
  {
    result_map tmp_map;
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _result_map.swap(tmp_map);
    }
    
    for (auto& tmp : tmp_map)
    {
      if (tmp.second != nullptr ) 
      {
        tmp.second( incoming_holder(nullptr) );
      }
    }
  }
  
  size_t remove_outdated()
  {
    size_t count = 0;
    auto calls = this->get_call_list();
    for ( auto call_id : calls ) 
    {
      if ( auto handler = this->detach(call_id) )
      {
        handler( incoming_holder(nullptr) );
        ++count;
      }
    }
    return count;
  }
  
private:
  
  call_list get_call_list()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    call_list calls;
    auto now = this->now_ms();
    while ( !_time_queue.empty() && _time_queue.top().first < now )
    {
      calls.push_back(_time_queue.top().second);
      _time_queue.pop();
    }
    return std::move( calls );
  }
  
  time_t now_ms()
  {
    return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

  }
  
private:
  bool _everytime = true;
  time_t _lifetime_ms = 1000;
  time_queue _time_queue;
  result_map _result_map;
  mutable mutex_type _mutex;
};

}} // wfc

