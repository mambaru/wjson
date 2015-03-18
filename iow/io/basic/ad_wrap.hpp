#pragma once

#include <iow/io/basic/tags.hpp>
#include <utility>

namespace iow{ namespace io{ namespace basic{
  
struct ad_wrap
{
  template<typename T, typename Handler>
  auto operator()(T& t, Handler&& h)
     -> decltype( t.get_aspect().template get<_owner_>().wrap( h ) )
  {
    return t.get_aspect().template get<_owner_>().wrap( std::forward<Handler>(h) );
  }

  template<typename T, typename Handler, typename NotAliveHandler>
  auto operator()(T& t, Handler&& h, NotAliveHandler&& nh)
    -> decltype( t.get_aspect().template get<_owner_>().wrap( h, nh ) )
  {
    return t.get_aspect().template get<_owner_>().wrap( std::forward<Handler>(h), std::forward<NotAliveHandler>(nh) );
  }
};
  
}}}
