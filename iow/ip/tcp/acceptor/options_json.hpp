#pragma once

#include <iow/ip/tcp/acceptor/options.hpp>
#include <iow/io/acceptor/options_json.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace acceptor{

struct options_json
{
  JSON_NAME(port)
  JSON_NAME(backlog)
  
  typedef json::object<
    options,
    json::member_list<
      json::base< ::iow::io::acceptor::options_json< ::iow::ip::tcp::connection::options > >,
      json::member< n_port, options, std::string, &options::port >,
      json::member< n_backlog, options, int, &options::backlog >
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}}

