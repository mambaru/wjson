#pragma once

#include <iow/jsonrpc/outgoing/outgoing.hpp>
#include <iow/jsonrpc/basic_types.hpp>

namespace iow{ namespace jsonrpc{

template<typename T>
struct outgoing_result
  : outgoing
{
  std::unique_ptr<T> result;
  ::iow::jsonrpc::data_ptr id;
};

  
}}

