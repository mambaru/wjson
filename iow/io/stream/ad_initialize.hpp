#pragma once

#include <iow/io/stream/tags.hpp>
#include <iow/io/reader/stream/tags.hpp>
#include <iow/io/writer/stream/tags.hpp>

namespace iow{ namespace io{ namespace stream{

struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O opt)
  {
    t.get_aspect().template get< ::iow::io::reader::stream::_initialize_ >()(t, opt.reader);
    t.get_aspect().template get< ::iow::io::writer::stream::_initialize_ >()(t, opt.writer);
  }
};

}}}
