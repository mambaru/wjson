#pragma once

#include <string>
#include <iow/jsonrpc/basic_types.hpp>

namespace iow{ namespace jsonrpc{

struct error
{
  error_code_t code;
  std::string message;

  error()
    : code()
    , message()
  {}

  error(error_code_t code, std::string message)
    : code(code)
    , message(message)
  {}

};

struct parse_error: error
{
  parse_error()
    : error(-32700, "Parse error.")
  {}
};

struct invalid_request: error
{
  invalid_request()
    : error(-32600, "Invalid Request.")
  {}
};

struct procedure_not_found: error
{
  procedure_not_found()
    : error(-32601, "Procedure not found.")
  {}
};

struct invalid_params: error
{
  invalid_params()
    : error(-32602, "Invalid params.")
  {}
};

struct internal_error: error
{
  internal_error()
    : error(-32603, "Internal error.")
  {}
};

struct method_not_impl: error
{
  method_not_impl()
    : error(-32001, "Method not implementation.")
  {}
};

// сетевая ошибка
struct bad_gateway: error
{
  bad_gateway()
    : error(-32002, "Bad Gateway.")
  {}
};

// ошибка конфигурации
struct service_unavailable: error
{
  service_unavailable()
    : error(-32003, "Service Unavailable.")
  {}
};


struct server_error: error
{
  server_error()
    : error(-32000, "Server error.")
  {}

  server_error( const std::string& what)
    : error(-32002, std::string("Server error: ") + what + ".")
  {}
};

}}
