#pragma once
#include <iow/jsonrpc/types.hpp>

namespace iow{ namespace jsonrpc{
  
template<typename Options>
struct engine_options_json
{
  typedef Options options_type;
  
  JSON_NAME(allow_non_jsonrpc)
  JSON_NAME(call_lifetime_ms)
  JSON_NAME(remove_outdated_ms)
  
  typedef wfc::json::object<
    options_type,
    ::wfc::json::member_list<
      ::wfc::json::member< n_allow_non_jsonrpc,  options_type, bool,   &options_type::allow_non_jsonrpc>,
      ::wfc::json::member< n_call_lifetime_ms,   options_type, time_t, &options_type::call_lifetime_ms>,
      ::wfc::json::member< n_remove_outdated_ms, options_type, time_t, &options_type::remove_outdated_ms>
    >
  > type;
  typedef typename type::target      target;
  typedef typename type::member_list member_list;
  typedef typename type::serializer  serializer;

};

}}
