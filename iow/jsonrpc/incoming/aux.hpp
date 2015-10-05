#pragma once

#include <iow/jsonrpc/types.hpp>

namespace iow{ namespace jsonrpc{ namespace aux{

void send_error( incoming_holder holder, std::unique_ptr<error>, outgoing_handler_t outgoing_handler);
  
void perform(
  data_ptr d, io_id_t io_id, outgoing_handler_t outgoing_handler, 
  std::function<void(incoming_holder, io_id_t, outgoing_handler_t)> incoming_handler );
  
}}}
