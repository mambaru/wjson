#pragma once

#include <iow/jsonrpc/outgoing/outgoing.hpp>
#include <iow/jsonrpc/basic_types.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{

template<typename T>
struct outgoing_error: outgoing
{
  typedef ::iow::jsonrpc::data_type data_type;
  std::unique_ptr<T> error;
  std::unique_ptr<data_type> id;
  outgoing_error()
    : outgoing()
    , id(nullptr)
  {}
};
}}

