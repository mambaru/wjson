#pragma once

#include <iow/io/reader/asio/tags.hpp>
#include <iow/system.hpp>

namespace iow{ namespace io{ namespace reader{ namespace asio{

struct ad_read_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, ::iow::system::error_code ec , std::size_t bytes_transferred)
  {
    if ( !ec )
    {
      p.second = bytes_transferred;
      t.get_aspect().template get< ::iow::io::reader::_complete_>()(t, std::move(p));
    }
    else
    {
      ///!! t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
    }
  }
};

}}}}
