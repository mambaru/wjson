#pragma once
#include <iostream>
#include <string>
#include <iow/system.hpp>
#include <iow/io/writer/tags.hpp>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace writer{ namespace asio{

// TODO: в descriptor?
struct ad_error_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, const ::iow::system::error_code& ec)
  {
    t.get_aspect().template get< ::iow::io::writer::_rollback_ >()(t, std::move(p));
    if ( ec == iow::system::errc::operation_would_block )
    {
      IOW_LOG_WARNING("iow::io::writer::asio::ad_error_handler (operation_would_block " 
                      << ec.value() << ") " << ec.message());
      t.get_aspect().template get<_more_>()(t);
    }
    else if ( ec != iow::system::errc::operation_canceled )
    {
      IOW_LOG_ERROR("iow::io::writer::asio::ad_error_handler ("
                      << ec.value() << ") " << ec.message());
      t.get_aspect().template gete< ::iow::io::_on_error_ >()(t, ec);
      t.get_aspect().template get< ::iow::io::_stop_>()(t);
    }
    else
    {
      IOW_LOG_WARNING("iow::io::writer::asio::ad_error_handler (operation_canceled): " << ec.message());
    }
  }
};

}}}}
