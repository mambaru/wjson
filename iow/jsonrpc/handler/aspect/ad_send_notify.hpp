#pragma once

#include <fas/aop.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
  
struct ad_send_notify
{
  template<typename T, typename Params, typename Serializer>
  void operator()(
    T& t,
    const char* name,
    Params params, 
    Serializer ser
  ) const
  {
    std::shared_ptr<Params> p = nullptr;
    
    if (params!=nullptr)
    {
      p = std::make_shared<Params>( std::move(params) );
    }
    
    t.send_notify(
      name,
      [p, ser](const char* name) -> typename T::data_ptr
      {
        return ser(name, std::move(*p));
      }
    );
    
  }
};

}} // iow


