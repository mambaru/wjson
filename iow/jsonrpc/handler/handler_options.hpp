#pragma once

#include <iow/jsonrpc/handler/handler_base_options.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{

template<typename T, typename P>
struct handler_options: handler_base_options
{
  T target; 
  P peeper;
};

}} // iow


