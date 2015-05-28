#pragma once

#include <iow/jsonrpc/outgoing/outgoing_result.hpp>
#include <iow/jsonrpc/outgoing/version_member.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace iow{ namespace jsonrpc{

template<typename T>
struct outgoing_result_json
{
  typedef typename T::target target;
  typedef outgoing_result<target> result_type;
  typedef typename result_type::version_type version_type;

  typedef ::iow::json::pointer<std::unique_ptr<target>, T> result_json;
  typedef ::iow::json::pointer< ::iow::io::data_ptr, json::raw_value< ::iow::io::data_type> > id_json;
  
  JSON_NAME(id)
  JSON_NAME(result)
  
  typedef ::iow::json::object<
    result_type,
    typename fas::type_list_n<
      version_member::type,
      ::iow::json::member<n_result, result_type, std::unique_ptr<target>,  &result_type::result, result_json >,
      ::iow::json::member<n_id,     result_type, ::iow::io::data_ptr, &result_type::id,   id_json >
    >::type
  > type;

  typedef typename type::serializer serializer;
};

}}

