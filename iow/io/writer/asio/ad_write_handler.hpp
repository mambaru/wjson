#pragma once
#include <iostream>
#include <string>
#include <iow/io/writer/tags.hpp>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace writer{ namespace asio{

// TODO: mv ad_write_complete
struct ad_write_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, ::iow::system::error_code ec , std::size_t bytes_transferred)
  {
    if ( !ec )
    {
      p.second = bytes_transferred;
      IOW_LOG_TRACE("WRITED[" << std::string(p.first, p.first + p.second) << "]" )
      t.get_aspect().template get< ::iow::io::writer::_complete_>()(t, std::move(p));
    }
    else
    {
      IOW_LOG_TRACE("WRITE ERROR: (" << ec.value() << ") " << ec.message() )
      abort();
      ///!! t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
    }
  }
};

}}}}
