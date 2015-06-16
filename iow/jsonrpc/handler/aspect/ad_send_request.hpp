#pragma once

#include <fas/aop.hpp>
#include <memory>
#include <iow/logger/logger.hpp>

namespace iow{ namespace jsonrpc{
  
struct ad_send_request
{
  template<typename T, typename Params, typename Serializer>
  void operator()(
    T& t,
    const char* name,
    Params params, 
    Serializer ser,
    typename T::result_handler_t  result_handler
  ) const
  {
    std::shared_ptr<Params> p = nullptr;
    if (params!=nullptr)
    {
      p = std::make_shared<Params>( std::move(params) );
    }
    
    /*
    if ( t.send_request == nullptr )
    {
      JSONRPC_LOG_FATAL("/handler/aspect/ad_send_request t.send_request == nullptr" )
      abort();
    }*/
    
    t.send_request(
      name,
      std::move(result_handler), 
      [p, ser, this](const char* name, typename T::call_id_t id) 
        -> typename T::data_ptr
      {
        return ser(name, std::move(*p), id);
      }
    );
  }
};

}} // iow


