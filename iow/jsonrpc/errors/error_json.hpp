#pragma once

#include <iow/jsonrpc/errors/errors.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>
#include <iow/io/types.hpp>

namespace iow{ namespace jsonrpc{

struct error_json
{
  JSON_NAME(code)
  JSON_NAME(message)

  typedef json::object<
    error,
    json::member_list<
      json::member<n_code,    error, error_code_t,      &error::code>,
      json::member<n_message, error, std::string,       &error::message>
    >
  > type;

  typedef error target; 
  typedef type::member_list member_list;
  typedef type::serializer  serializer;
};

}}
