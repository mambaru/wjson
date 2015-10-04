#pragma once

#include <iow/jsonrpc/outgoing/outgoing_error.hpp>
#include <iow/jsonrpc/outgoing/version_member.hpp>
#include <iow/jsonrpc/basic_types.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace iow{ namespace jsonrpc{

template<typename T>
struct outgoing_error_json
{
  typedef ::iow::jsonrpc::data_type data_type;
  typedef ::iow::jsonrpc::data_ptr  data_ptr;
  typedef typename T::target target;
  typedef outgoing_error<target> error_type;
  typedef typename error_type::version_type version_type;
  typedef ::iow::json::pointer<std::unique_ptr<target>, T> error_json;
  typedef ::iow::json::pointer<
              std::unique_ptr<data_type>, 
              ::iow::json::raw_value< data_type> 
          > id_json;
  
  JSON_NAME(error)
  JSON_NAME(id)
  
  typedef ::iow::json::object<
    error_type,
    json::member_list<
      version_member::type,
      ::iow::json::member<n_error, error_type, std::unique_ptr<target>, &error_type::error, error_json >,
      ::iow::json::member<n_id,    error_type, data_ptr, &error_type::id,  id_json >
    >
  > type;

  typedef typename type::serializer serializer;
};

}}

