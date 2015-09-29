#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
  
struct ad_invoke_error
{
  template<
    typename T,
    typename Holder, 
    typename Error, 
    typename OutgoingHandler
  >
  void operator()(T&, Holder holder, Error e, OutgoingHandler handler)
  {
    T::aspect::template advice_cast<_send_error_>::type
      ::template send<T, error_json>( 
          std::move(holder), 
          std::move(e), 
          std::move(handler) 
      );
  }
};


}} // iow


