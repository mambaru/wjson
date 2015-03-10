#pragma once

#include <iow/basic/io_context.hpp>
#include <memory>
#include <functional>

namespace iow{
  
template<typename DataType, typename DataPtr = std::unique_ptr<DataType> >
struct descriptor_context: io_context
{
  typedef DataType data_type;
  typedef DataPtr  data_ptr;
  
  typedef std::function< void(data_ptr) > outgoing_handler_t;
  typedef std::function< void(io_id_t, data_ptr, outgoing_handler_t) > incoming_handler_t;
  typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
  typedef std::function< void(io_id_t) > shutdown_handler_t;
  
  outgoing_handler_t outgoing_handler;
  incoming_handler_t incoming_handler;
  startup_handler_t  startup_handler;
  shutdown_handler_t shutdown_handler;
  
  void reset()
  {
    io_context::reset();
  }
};

}