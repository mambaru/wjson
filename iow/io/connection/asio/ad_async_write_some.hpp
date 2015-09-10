#pragma once

#include <iow/asio.hpp>
#include <utility>
#include <iow/logger/logger.hpp>
namespace iow{ namespace io{ namespace connection{ namespace asio{

struct ad_async_write_some
{
  template<typename T, typename P, typename H>
  void operator()(T& t, P p, H&& handler)
  {
    t.descriptor().async_write_some(
      ::boost::asio::buffer( p.first, p.second ),
      std::forward<H>(handler)
    );
  }
};

}}}}
