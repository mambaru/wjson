#pragma once
#include <functional>
#include <iow/io/basic/context.hpp>

namespace iow{ namespace io{ namespace descriptor{

template<typename ID, typename DataType, typename DataPtr>
struct context
{
  typedef ID io_id_t;
  typedef DataType data_type;
  typedef DataPtr  data_ptr;
  
  typedef std::function< void(data_ptr) > outgoing_handler_t;
  typedef std::function< void(data_ptr, io_id_t, outgoing_handler_t) > incoming_handler_t;
  typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
  typedef std::function< void(io_id_t) > shutdown_handler_t;

  outgoing_handler_t outgoing_handler;
  incoming_handler_t incoming_handler;
  startup_handler_t  startup_handler;
  shutdown_handler_t shutdown_handler;
};

}}}
