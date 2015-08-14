#pragma once

#include <iow/ip/tcp/client/options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

struct options_json
{
  JSON_NAME(port)
  JSON_NAME(connection)
  
  typedef ::iow::ip::tcp::connection::options connection_options;
  
  typedef json::object<
    options,
    json::member_list<
      json::member< n_port,       options, std::string, &options::port >,
      json::member< n_connection, options, connection_options, &options::backlog >
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}}

