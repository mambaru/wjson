#pragma once

#include <iow/jsonrpc/handler/handler_types.hpp>

namespace iow{ namespace jsonrpc{

struct handler_base_options
{
  typedef handler_types::sender_handler_t sender_handler_t;
  sender_handler_t sender_handler;
};

}} // iow


