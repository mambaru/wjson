#pragma once

#include <string>
#include <iow/io/client/options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace io{ namespace client{

template< typename ConnectionOptionsJson >
struct options_json
{
  typedef ConnectionOptionsJson connection_json;
  typedef typename connection_json::target connection_options;
  typedef options<connection_options> option_type;
  
  JSON_NAME(async_connect)
  JSON_NAME(addr)
  JSON_NAME(port)
  JSON_NAME(threads)
  JSON_NAME(connection)
  JSON_NAME(reconnect_timeout_ms)
  JSON_NAME(connect_count)
  
  typedef json::object<
    option_type,
    json::member_list<
      json::member< n_connection, option_type, connection_options, &option_type::connection, connection_json>,
      json::member< n_async_connect, option_type, bool, &option_type::async_connect>,
      json::member< n_addr, option_type, std::string, &option_type::addr>,
      json::member< n_port, option_type, std::string, &option_type::port>,
      json::member< n_threads, option_type, int, &option_type::threads>,
      json::member< n_reconnect_timeout_ms, option_type, time_t, &option_type::reconnect_timeout_ms>,
      json::member<n_connect_count, option_type, int, &option_type::connect_count>
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}}}
