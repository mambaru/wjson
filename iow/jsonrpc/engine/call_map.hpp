#pragma once

#include <fas/aop.hpp>
#include <iow/mutex.hpp>
#include <mutex>


namespace iow{ namespace jsonrpc{

class call_map
{
public:
  typedef std::function<void(incoming_holder holder)> result_handler_t;
  typedef outgoing_holder::call_id_t call_id_t; 
  typedef std::map<call_id_t, result_handler_t> result_map;
  typedef std::mutex mutex_type;

  void set(call_id_t call_id, result_handler_t result)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _result_map[call_id] = result;
  }
  
  result_handler_t detach(call_id_t call_id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    /*
    std::stringstream ss;
    ss << "detach call_id=" << call_id << " [";
    for ( const auto& id : _result_map)
      ss << id.first << ", ";
    ss << "]";
    std::cout << ss.str() << std::endl;
    */
    
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
  
private:
  result_map _result_map;
  mutable mutex_type _mutex;
};

}} // wfc

