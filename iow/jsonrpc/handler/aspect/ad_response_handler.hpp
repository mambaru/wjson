#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
  
struct ad_response_handler
{
  template<
    typename Tg,
    typename Result,
    typename Error
  >
  static inline void process(
    Result res,
    Error err,
    std::function<void(Result)> result_callback,
    std::function<void(Error)> error_callback
  )
  {
    if ( res!=nullptr )
    {
      if (result_callback!=nullptr)
      {
        result_callback( std::move(res) );
      }
    }
    else if ( err!=nullptr )
    {
      if (error_callback!=nullptr)
      {
        error_callback( std::move(err) );
      }
      else if ( result_callback!=nullptr )
      {
        result_callback( nullptr );
      }
    }
    else if (result_callback!=nullptr)
    {
      result_callback(nullptr);
    }
  }
};

}} // iow


