#pragma once

#include <iow/io/reader/asio/tags.hpp>
#include <iow/io/reader/tags.hpp>
#include <iow/io/basic/tags.hpp>

#include <iow/logger/logger.hpp>
#include <iow/system.hpp>
#include <iow/asio.hpp>

namespace iow{ namespace io{ namespace reader{ namespace asio{

struct ad_error_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, ::iow::system::error_code ec)
  {
    t.get_aspect().template get< ::iow::io::reader::_rollback_>()(t, std::move(p));
    t.get_aspect().template gete< ::iow::io::_on_error_ >()(t, ec);
    t.get_aspect().template get< ::iow::io::_stop_>()(t);
  }
};
  
}}}}
