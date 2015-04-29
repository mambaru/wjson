#pragma once

#include <memory>

#include <fas/aop.hpp>
#include <fas/integral/bool_.hpp>

namespace iow{ namespace io{ namespace descriptor{ 

// Универсальный для всех
template<bool MakeOutgoingHandler, typename TgInitialize>
struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O&& opt) 
  {
    typedef typename T::aspect::template advice_cast< _context_>::type context_type;
    context_type& cntx = t.get_aspect().template get<_context_>();
    if (  opt.incoming_handler != nullptr )
    {
      make_outgoing(t, cntx, fas::bool_<MakeOutgoingHandler>() );
    }

    cntx.incoming_handler  = opt.incoming_handler;
    cntx.startup_handler   = opt.startup_handler;
    cntx.shutdown_handler  = opt.shutdown_handler;

    t.get_aspect().template get< TgInitialize >()( t, std::forward<O>(opt) );
  }

private:

  template<typename T, typename Cntx> 
  void make_outgoing(T& t, Cntx& cntx, fas::true_ )
  {
    typedef Cntx context_type;
    auto callback = cntx.outgoing_handler;
    std::weak_ptr<T> wthis = t.shared_from_this();
    cntx.outgoing_handler = t.wrap([wthis, callback](typename context_type::data_ptr d)
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk( pthis->mutex() );
        if ( callback != nullptr )
        {
          callback(std::move(d));
        }
        else
        {
          pthis->get_aspect().template get<_outgoing_>()( *pthis, std::move(d) );
        }
      }
    });
  }

  template<typename T, typename Cntx>
  void make_outgoing(T& t, Cntx& cntx, fas::false_ )
  {
    cntx.outgoing_handler = nullptr;
  }

};

  
}}}
