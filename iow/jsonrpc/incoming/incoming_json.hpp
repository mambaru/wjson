#pragma once

#include <iow/jsonrpc/incoming/incoming.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>


namespace iow{ namespace jsonrpc{

struct incoming_json
{
  typedef incoming::pair_type pair_type;
  typedef json::raw_pair<pair_type> pair_json;

  JSON_NAME(id)
  JSON_NAME(method)
  JSON_NAME(params)
  JSON_NAME(result)
  JSON_NAME(error)

  typedef json::object<
    incoming, 
    json::member_list<
      json::member<n_method, incoming, pair_type, &incoming::method, pair_json>,
      json::member<n_params, incoming, pair_type, &incoming::params, pair_json>,
      json::member<n_result, incoming, pair_type, &incoming::result, pair_json>,
      json::member<n_error,  incoming, pair_type, &incoming::error,  pair_json>,
      json::member<n_id,     incoming, pair_type, &incoming::id,     pair_json>
    >
  > type;

  typedef type::target target;
  typedef type::member_list member_list;
  typedef type::serializer  serializer;

};

  
}}
