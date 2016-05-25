#pragma once

#include <iow/asio.hpp>
#include <utility>

namespace iow{ namespace io{ namespace connection{ namespace asio{

struct ad_async_read_some
{
  template<typename T, typename P, typename H>
  void operator()(T& t, P p, H&& handler)
  {
    auto tmp = [handler]( ::iow::system::error_code ec , std::size_t bytes_transferred )
    {
      handler(ec, bytes_transferred);
    };
    
    t.descriptor().async_read_some(
      ::iow::asio::buffer( p.first, p.second ),
      std::forward<H>(tmp)
    );
  }
};

}}}}
