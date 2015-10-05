#pragma once
#include <iow/jsonrpc/types.hpp>

namespace iow{ namespace jsonrpc{
  
template<typename HandlerOptions>
struct engine_options: HandlerOptions
{
  typedef HandlerOptions handler_options;
  outgoing_handler_t outgoing_handler;
  bool direct_mode = false;
};

}}
