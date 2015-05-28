#pragma once

#include <iow/jsonrpc/outgoing/outgoing.hpp>
#include <memory>
#include <string>

namespace iow{ namespace jsonrpc{

template<typename T>
struct outgoing_request
  : outgoing
{
  // TODO: оптимизоровать method, на const char (см. json)
  std::string method;
  std::unique_ptr<T> params;
  int id;
  outgoing_request()
    : outgoing()
    , id(-1)
  {
  }
};

}}

