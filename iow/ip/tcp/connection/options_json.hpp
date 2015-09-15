#pragma once

#include <iow/ip/tcp/connection/options.hpp>

#include <iow/io/stream/options_json.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace connection{
  
struct options_json
{
  JSON_NAME(keep_alive)
  typedef options::data_type data_type;
  
  typedef json::object<
    options,
    json::member_list<
        json::base< ::iow::io::stream::options_json<data_type> >,
        json::member< n_keep_alive, options, bool, &options::keep_alive>
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


}}}}

