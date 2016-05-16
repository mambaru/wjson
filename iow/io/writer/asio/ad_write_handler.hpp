#pragma once
#include <iostream>
#include <string>
#include <iow/io/writer/tags.hpp>
#include <iow/io/writer/asio/tags.hpp>
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
      IOW_LOG_TRACE("WRITED[" << std::string(p.first, p.first + p.second) << "] id=" << t.get_id_(t) )
      t.get_aspect().template get< ::iow::io::writer::_complete_>()(t, std::move(p));
    }
    else
    {
      p.second = 0;
      t.get_aspect().template get< _error_handler_>()(t, std::move(p), std::move(ec));
    }
  }
};

}}}}
