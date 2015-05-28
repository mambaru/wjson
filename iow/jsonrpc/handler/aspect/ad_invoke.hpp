#pragma once

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
  template<typename T>
  void operator()(
    T& t,
    typename T::holder_type holder,
    typename T::outgoing_handler_t outgoing_handler
  ) 
  {
    typedef typename T::holder_type holder_type;
    typedef typename T::outgoing_handler_t outgoing_handler_t;

    typedef f_invoke<holder_type, outgoing_handler_t> f;
    
    if ( !fas::for_each_group<_method_>(t, f( holder, outgoing_handler ) ) )
    {
      T::aspect::template advice_cast<_send_error_>::type
           ::template send<T, error_json>( 
              std::move(holder), 
              std::make_unique<procedure_not_found>(), 
              std::move(outgoing_handler) 
           );
    }
  }

};

}} // iow


