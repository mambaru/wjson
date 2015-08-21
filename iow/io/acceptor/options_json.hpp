#pragma once

#include <string>
#include <iow/io/acceptor/options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace io{ namespace acceptor{

template<typename ConnectionsOptionsJson>
struct options_json
{
  typedef ConnectionsOptionsJson connection_options_json;
  typedef typename connection_options_json::target connection_options_type;
  typedef options<connection_options_type> options_type;
  
  JSON_NAME(max_connections)
  JSON_NAME(host)
  JSON_NAME(connection)
  
  typedef json::object<
    options_type,
    json::member_list<
      json::member< n_connection, options_type, connection_options_type, &options_type::connection, connection_options_json>,
      json::member< n_max_connections, options_type, int, &options_type::max_connections >,
      json::member< n_host, options_type, std::string, &options_type::host >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}}}
