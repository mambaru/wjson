#pragma once

#include <iow/logger/logger.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
  

template<typename HolderType, typename OutgoingHandler>
struct f_invoke
{
  typedef HolderType holder_type;
  typedef OutgoingHandler outgoing_handler_t;
  
  holder_type& holder;
  outgoing_handler_t& outgoing_handler;
  bool founded;
  
  f_invoke(holder_type& holder, outgoing_handler_t& outgoing_handler)
    : holder( holder )
    , outgoing_handler(outgoing_handler)
    , founded(false)
  {
  }
  
  operator bool () const
  {
    return founded;
  }
  
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    if ( !founded && holder )
    {
      if ( holder.method_equal_to( t.get_aspect().template get<Tg>().name() ) )
      {
        t.get_aspect().template get<Tg>()(t, std::move(holder), outgoing_handler );
        founded = true;
      }
    }
  }
};

struct ad_invoke
{
  template<typename T, typename HolderType, typename OutgoingHandler>
  void operator()(
    T& t,
    HolderType holder,
    OutgoingHandler outgoing_handler
  )
  {
    typedef HolderType holder_type;
    typedef OutgoingHandler outgoing_handler_t;
    typedef f_invoke<holder_type, outgoing_handler_t> f;

    if ( !holder.has_method() )
    {
      JSONRPC_LOG_WARNING( "jsonrpc::invoke: Invalid Request " )
      t.get_aspect().template get<_invoke_error_>()
        (t, std::move(holder), std::make_unique<invalid_request>(), std::move(outgoing_handler));
      return;
    }

    if ( !fas::for_each_group<_method_>(t, f( holder, outgoing_handler ) ) )
    {
      JSONRPC_LOG_WARNING( "jsonrpc::invoke: Procedure Not Found: " << holder.method() )

      t.get_aspect().template get<_invoke_error_>()
        (t, std::move(holder), std::make_unique<procedure_not_found>(), std::move(outgoing_handler));
    }
  }

};

}} // iow


