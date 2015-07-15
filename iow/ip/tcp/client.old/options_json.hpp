#pragma once

#include <iow/ip/tcp/client/options.hpp>
#include <iow/ip/tcp/connection/options_json.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{
  
struct options_json
{
  JSON_NAME(host)
  JSON_NAME(port)
  
  typedef json::object<
    options,
    json::member_list<
      json::base< ::iow::ip::tcp::connection::options_json >,
      json::member< n_host, options, std::string, &options::host >,
      json::member< n_port, options, std::string, &options::port >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


}}}}

