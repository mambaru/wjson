#pragma once

#include <iow/io/basic/aspect.hpp>
#include <iow/io/pipe/ad_output.hpp>
#include <iow/io/pipe/ad_more.hpp>
#include <iow/io/pipe/ad_complete.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace descriptor{ namespace stream{

  
struct ad_async_read_some
{
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    if ( !t.status() )
      return;
    
    std::weak_ptr<T> wthis = t.shared_from_this();
    auto handler = t.wrap([wthis, p]( iow::system::error_code ec , std::size_t bytes_transferred )
    { 
      if ( auto pthis = pthis.lock() )
      {
        typename T::lock_guard lk(pthis->mutex());
        pthis->get_aspect().template get<_read_handler_>()(*pthis, std::move(p), std::move(ec), bytes_transferred);
      }
    });
    
    t.descriptor().async_read_some( ::iow::asio::buffer( p.first, p.second ), std::move(handler) );
  }
};

  
struct aspect: fas::aspect<
>{};
  
  

}}}}
