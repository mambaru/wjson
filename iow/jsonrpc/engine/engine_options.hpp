#pragma once
#include <iow/jsonrpc/types.hpp>

namespace iow{ namespace jsonrpc{
  
template<typename HandlerOptions>
struct engine_options: HandlerOptions
{
  typedef HandlerOptions handler_options;
  // Исходящий jsonrpc хандлер для запросов и ответов
  outgoing_handler_t rpc_send_handler;
  // Исходящий "сырой" io хандлер для запросов
  ::iow::io::incoming_handler_t io_send_handler;
  // Исходящий "сырой" io хандлер для ответов
  // Инициализируться исключительно при запросе
  // Юзер значение сбрасываеться (TODO: обертка на юзер нандлером)
  // ::iow::io::outgoing_handler_t io_response_handler;
  bool direct_mode = false;
};

}}
