#pragma once

#include <fas/aop.hpp>
#include <memory>
#include <iow/logger/logger.hpp>
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>

namespace iow{ namespace jsonrpc{
 
struct ad_perform_send
{
  template<typename T, typename Params, typename NotifySerializer, typename RequestSerializer, typename ResultHandler >
  void operator()(
    T& t,
    const char* name,
    Params params, 
    NotifySerializer ns1,
    RequestSerializer rs1,
    ResultHandler rh
  ) const
  {
    IOW_LOG_DEBUG(" DEBUG: --- handler ad_perform_send --- 1 - ")

    std::shared_ptr<Params> p = nullptr;

    if (params!=nullptr)
    {
      p = std::make_shared<Params>( std::move(params) );
    }

    IOW_LOG_DEBUG(" DEBUG: --- handler ad_perform_send --- 1 - ")
    t.sender_handler(
      name,
      [p, ns1](const char* name) -> typename T::data_ptr
      {
        IOW_LOG_DEBUG(" DEBUG: --- handler ad_perform_send --- 1.1 - ")
        return ns1(name, std::move(*p));
      },
      [p, rs1, this](const char* name, typename T::call_id_t id) 
        -> typename T::data_ptr
      {
        IOW_LOG_DEBUG(" DEBUG: --- handler ad_perform_send --- 1.2 - ")
        return rs1(name, std::move(*p), id);
      },
      std::move(rh)
    );
    IOW_LOG_DEBUG(" DEBUG: --- handler ad_perform_send --- 2 - ")

    
  }
  
};
  
  /*
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
*/

}} // iow


