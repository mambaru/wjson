#pragma once

#include <iow/asio.hpp>
#include <utility>

namespace iow{ namespace io{ namespace connection{ namespace asio{

struct ad_async_read_some
{
  template<typename T, typename P, typename H>
  void operator()(T& t, P p, H&& handler)
  {
    IOW_LOG_DEBUG("================= ad_async_read_some =======================");
    auto tmp = [handler]( ::iow::system::error_code ec , std::size_t bytes_transferred )
    {
      IOW_LOG_DEBUG("================= TMP!!! ad_async_read_some =======================")
      handler(ec, bytes_transferred);
      IOW_LOG_DEBUG("================= TMP!!! ad_async_read_some READY=======================")
    };
    
    t.descriptor().async_read_some(
      ::iow::asio::buffer( p.first, p.second ),
      std::forward<H>(tmp)
    );
  }
};

}}}}
