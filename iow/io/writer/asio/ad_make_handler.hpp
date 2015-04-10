#pragma once

#include <iow/io/writer/asio/tags.hpp>

namespace iow{ namespace io{ namespace writer{ namespace asio{

struct ad_make_handler
{
  typedef std::function<void(::iow::system::error_code, std::size_t)> handler_type;
  
  template<typename T, typename P>
  handler_type operator()(T& t, const P& p) 
  {
    std::weak_ptr<T> wthis = t.shared_from_this();
    return t.wrap([wthis, p]( ::iow::system::error_code ec , std::size_t bytes_transferred )
    { 
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_write_handler_>()(*pthis, std::move(p), std::move(ec), bytes_transferred);
      }
    });
  }
};

}}}}
