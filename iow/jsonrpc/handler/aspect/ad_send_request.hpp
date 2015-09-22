#pragma once

#include <fas/aop.hpp>
#include <memory>
#include <iow/logger/logger.hpp>
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>

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
    
    /*{
      auto ser2 = [name, p, ser](typename T::call_id_t id)->data_ptr
      {
        return ser(name, std::move(*p), id);
      };
      outgoing_holder oh( name, std::move(ser2), result_handler);
       //outgoing_holder(const char* name, request_serializer_t serializer, result_handler_t result_handler, time_point_t time_point = time_point_t())
 
    }*/

    JSONRPC_LOG_DEBUG("-1- iow::jsonrpc::ad_send_request t.send_request " )
    t.send_request(
      name,
      std::move(result_handler), 
      [p, ser, this](const char* name, typename T::call_id_t id) 
        -> typename T::data_ptr
      {
        return ser(name, std::move(*p), id);
      }
    );
    JSONRPC_LOG_DEBUG("-2- iow::jsonrpc::ad_send_request t.send_request " )
  }
};

}} // iow


