#pragma once

#include <string>
#include <iow/io/acceptor/options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace io{ namespace acceptor{

template<typename AcceptorOptions, typename ConnectionsOptionsJson>
struct options_json
{
  typedef AcceptorOptions options_type;
  typedef ConnectionsOptionsJson connection_options_json;
  typedef typename connection_options_json::target connection_options_type;
  
  JSON_NAME(max_connections)
  JSON_NAME(connection)
  JSON_NAME(addr)
  JSON_NAME(port)
  JSON_NAME(backlog)
  
  typedef json::object<
    options_type,
    json::member_list<
      json::member< n_connection, options_type, connection_options_type, &options_type::connection, connection_options_json>,
      json::member< n_max_connections, options_type, int, &options_type::max_connections >,
      json::member< n_addr, options_type, std::string, &options_type::addr>,
      json::member< n_port, options_type, std::string, &options_type::port>,
      json::member< n_backlog, options_type, int, &options_type::backlog >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}}}
