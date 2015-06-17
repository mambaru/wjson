#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <iow/system.hpp>
#include <functional>
#include <memory>
#include <mutex>

namespace iow{ namespace io{ namespace acceptor{


struct ad_make_handler
{
  typedef std::function<void(::iow::system::error_code)> handler_type;
  
  template<typename T, typename P>
  handler_type operator()(T& t, const P& p) 
  {
    
    std::weak_ptr<T> wthis = t.shared_from_this();
    return t.wrap([wthis, p]( ::iow::system::error_code ec )
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_accept_handler_>()(*pthis, std::move(p), std::move(ec));
      }
    });
  }
};

  
}}}
