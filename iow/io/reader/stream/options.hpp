#pragma once

#include <iow/io/aux/read_buffer.hpp>

namespace iow{ namespace io{ namespace reader{ namespace stream{
  
template<typename DataType>
struct options:
  ::iow::io::read_buffer_options<DataType>
{
  size_t maxsize = 1024*1024*128;
  size_t wrnsize = 1024*1024;
};

}}}}
