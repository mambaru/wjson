#pragma once

#include <iow/io/reader/tags.hpp>
#include <iow/system.hpp>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace acceptor{

struct ad_accept_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, const ::iow::system::error_code& ec)
  {

    if ( !ec )
    {
      t.get_aspect().template get< ::iow::io::reader::_complete_ >()(t, std::move(p));
    }
    else if ( ec != iow::system::errc::operation_canceled )
    {
      IOW_LOG_FATAL("Acceptor error: (" << ec.value() << ") " << ec.message() )
      abort();
#warning abort();
      ///!! t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
      //
    }
  }
};

  
}}}
