#pragma once

#include <iow/ip/tcp/connection/options_json.hpp>
#include <iow/ip/tcp/acceptor/options_json.hpp>
#include <iow/ip/tcp/server/options.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{
  
struct options_json
{
  typedef ::iow::ip::tcp::acceptor::options_json acceptor_option_json;
  typedef ::iow::ip::tcp::acceptor::options acceptor_options;
  
  typedef ::iow::ip::tcp::connection::options_json connection_option_json;
  typedef ::iow::ip::tcp::connection::options connection_options;
  
  
  JSON_NAME(connection)
  JSON_NAME(acceptor)
  typedef json::object<
    options,
    json::member_list<
      json::base< acceptor_option_json >
      /*json::member<n_acceptor,   options, acceptor_options, &options::acceptor, acceptor_option_json>,
      json::member<n_connection, options, connection_options, &options::connection, connection_option_json>
      */
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}}

