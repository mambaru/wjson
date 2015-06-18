#pragma once

/*
#include <iow/io/aux/read_buffer.hpp>
#include <iow/io/aux/write_buffer.hpp>

#define IOW_STREAM_DEFAULT_MAXSIZE (1024*1024*128)
#define IOW_STREAM_DEFAULT_WRNSIZE (1024*1024)
*/

#include <iow/io/reader/stream/options.hpp>
#include <iow/io/writer/stream/options.hpp>

namespace iow{ namespace io{ namespace stream{
  
/*
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
*/

template<typename DataType>
struct options
{
  typedef ::iow::io::reader::stream::options<DataType> reader_options;
  typedef ::iow::io::writer::stream::options<DataType> writer_options;
  reader_options reader;
  writer_options writer;
};

}}}
