#pragma once

#include <iow/jsonrpc/handler/handler_types.hpp>

namespace iow{ namespace jsonrpc{

struct handler_base_options
{
  typedef handler_types::sender_handler_t sender_handler_t;
  sender_handler_t sender_handler;
  
  /*
  typedef handler_types::send_request_t send_request_t;
  typedef handler_types::send_notify_t send_notify_t;

  send_request_t send_request = nullptr;
  send_notify_t  send_notify = nullptr; 
  */
};

}} // iow


