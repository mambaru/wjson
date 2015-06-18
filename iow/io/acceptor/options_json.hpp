#pragma once

#include <string>
#include <iow/io/acceptor/options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace io{ namespace acceptor{

template<typename ConnectionsOptions>
struct options_json
{
  typedef options<ConnectionsOptions> options_type;
  
  JSON_NAME(max_connections)
  JSON_NAME(host)
  
  typedef json::object<
    options_type,
    json::member_list<
      json::member< n_max_connections, options_type, int, &options_type::max_connections >,
      json::member< n_host, options_type, std::string, &options_type::host >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}}}
