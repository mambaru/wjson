#pragma once

#include <iow/io/mtio/options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace io{ namespace mtio{
  
struct options_json
{
  JSON_NAME(threads)
  
  typedef json::object<
    options,
    json::member_list<
      json::member<n_threads, options, int, &options::threads>
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}

