#pragma once

#include <iow/io/reader/tags.hpp>
#include <iow/system.hpp>

namespace iow{ namespace io{ namespace acceptor{

struct ad_accept_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, ::iow::system::error_code ec)
  {

    if ( !ec )
    {
      t.get_aspect().template get< ::iow::io::reader::_complete_ >()(t, std::move(p));
    }
    else
    {
      ///!! t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
      //
    }
  }
};

  
}}}
