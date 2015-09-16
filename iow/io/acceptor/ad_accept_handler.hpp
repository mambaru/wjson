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
    else if ( ec == iow::system::errc::operation_would_block)
    {
      t.get_aspect().template get< ::iow::io::reader::_more_ >()(t);
    }
    else if ( ec != iow::system::errc::operation_canceled )
    {
      IOW_LOG_FATAL("ad_accept_handler " << ec.message())
      const auto& context = t.get_aspect().template get<_context_>();
      context.fatal_handler(ec.value(), ec.message());
    }
  }
};

  
}}}
