#pragma once

#include <iow/io/aux/read_buffer.hpp>
#include <iow/io/aux/write_buffer.hpp>

#define IOW_STREAM_DEFAULT_MAXSIZE (1024*1024*128)
#define IOW_STREAM_DEFAULT_WRNSIZE (1024*1024)

namespace iow{ namespace io{ namespace stream{
  
  
template<typename DataType>
struct reader_options:
  ::iow::io::read_buffer_options<DataType>
{
  size_t maxsize = IOW_STREAM_DEFAULT_MAXSIZE;
  size_t wrnsize = IOW_STREAM_DEFAULT_WRNSIZE;
};

template<typename DataType>
struct writer_options:
  ::iow::io::write_buffer_options<DataType>
{
  size_t maxsize = IOW_STREAM_DEFAULT_MAXSIZE;
  size_t wrnsize = IOW_STREAM_DEFAULT_WRNSIZE;
};

template<typename DataType>
struct stream_options
{
  reader_options<DataType> reader;
  writer_options<DataType> writer;
};

}}}
