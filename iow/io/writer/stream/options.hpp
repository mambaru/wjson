#pragma once

#include <iow/io/aux/write_buffer.hpp>

#define IOW_WRISTREAM_DEFAULT_MAXSIZE (1024*1024*128)
#define IOW_STREAM_DEFAULT_WRNSIZE (1024*1024)

namespace iow{ namespace io{ namespace writer{ namespace stream{
  

template<typename DataType>
struct writer_options:
  ::iow::io::write_buffer_options<DataType>
{
  // Вынести на более высокий уровень
  size_t maxsize = 1024*1024*128;
  size_t wrnsize = 1024*1024;
};

}}}}
