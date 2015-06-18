#pragma once

#include <iow/io/aux/write_buffer_options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace io{
    
template<typename DataType>  
struct write_buffer_options_json
{
  typedef DataType data_type;
  typedef ::iow::io::write_buffer_options<data_type>  options_type;

  JSON_NAME(sep)
  JSON_NAME(bufsize)
  JSON_NAME(maxbuf)
  JSON_NAME(minbuf)
  JSON_NAME(first_as_is)

  typedef json::object<
    options_type,
    json::member_list<
      json::member< n_sep,      options_type, std::string, &options_type::sep     >,
      json::member< n_bufsize,  options_type, size_t,      &options_type::bufsize >,
      json::member< n_maxbuf,   options_type, size_t,      &options_type::maxbuf  >,
      json::member< n_minbuf,   options_type, size_t,      &options_type::minbuf  >,
      json::member< n_first_as_is,  options_type, bool,        &options_type::first_as_is >
    >
  > type;
  
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}}
