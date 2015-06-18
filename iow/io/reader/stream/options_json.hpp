#pragma once

#include <iow/io/reader/stream/options.hpp>
#include <iow/io/aux/read_buffer_options_json.hpp>

namespace iow{ namespace io{ namespace reader{ namespace stream{

template<typename DataType>  
struct options_json
{
  typedef DataType data_type;
  typedef ::iow::io::reader::stream::options<data_type>  options_type;
  typedef ::iow::io::read_buffer_options_json<data_type> buffer_json;

  typedef json::object<
    options_type,
    json::member_list<
      json::base<buffer_json>
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}}}}
