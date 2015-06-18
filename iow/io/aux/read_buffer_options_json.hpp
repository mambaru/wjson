#pragma once

#include <iow/io/aux/read_buffer_options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace io{
    
template<typename DataType>  
struct read_buffer_options_json
{
  typedef DataType data_type;
  typedef ::iow::io::read_buffer_options<data_type>  options_type;
  
  JSON_NAME(sep)
  JSON_NAME(bufsize)
  JSON_NAME(maxbuf)
  JSON_NAME(minbuf)
  JSON_NAME(trimsep)

  typedef json::object<
    options_type,
    json::member_list<
      json::member< n_sep,      options_type, std::string, &options_type::sep     >,
      json::member< n_bufsize,  options_type, size_t,      &options_type::bufsize >,
      json::member< n_maxbuf,   options_type, size_t,      &options_type::maxbuf  >,
      json::member< n_minbuf,   options_type, size_t,      &options_type::minbuf  >,
      json::member< n_trimsep,  options_type, bool,        &options_type::trimsep >
    >
  > type;
  
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

/*

template<typename DataType>
struct read_buffer_options
{
  typedef std::unique_ptr<DataType> data_ptr;
  typedef std::function< data_ptr(size_t, size_t) > create_fun;
  typedef std::function< void(data_ptr) > free_fun;

  std::string sep;
  size_t bufsize=4096;
  size_t maxbuf=4096*2;
  size_t minbuf=0;
  bool fast_mode = false;
  bool trimsep = false; // Отрезать сепаратор 
  std::function< data_ptr(size_t, size_t) > create;
  std::function< void(data_ptr) > free;
};
*/

}}
