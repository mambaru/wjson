#pragma once

#include <fas/aop.hpp>
#include <iow/mutex.hpp>
#include <mutex>


namespace iow{ namespace jsonrpc{

class call_registry
{
public:
  typedef std::function<void(incoming_holder holder)> result_handler_t;
  typedef outgoing_holder::call_id_t call_id_t; 
  typedef std::map<call_id_t, result_handler_t> result_map;

  void set(call_id_t call_id, result_handler_t result)
  {
    if (result==nullptr)
    {
      std::cout << "TMP call_registry FATAL" << std::endl;
      abort();
    }
    _result_map[call_id] = result;
  }
  
  result_handler_t detach(call_id_t call_id)
  {
    result_handler_t result = nullptr;
    auto itr = _result_map.find(call_id);
    if ( itr!=_result_map.end() )
    {
      result = std::move( itr->second );
      _result_map.erase(itr);
    }
    return std::move(result);
  }
  
private:
  
  result_map _result_map;
};

}} // wfc

