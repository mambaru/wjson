#pragma once
//#include <functional>
#include <iow/io/basic/context.hpp>
#include <iow/io/types.hpp>

namespace iow{ namespace io{ namespace descriptor{

template<typename ID, typename DataType, typename DataPtr>
struct context
{
  typedef ID       io_id_type;
  typedef DataType data_type;
  typedef DataPtr  data_ptr;

  typedef std::function< void(data_ptr) > outgoing_handler_type;
  typedef std::function< void(data_ptr, io_id_type, outgoing_handler_type )> incoming_handler_type;
  typedef std::function< void(io_id_type, outgoing_handler_type) > startup_handler_type;
  typedef std::function< void(io_id_type) > shutdown_handler_type;
  typedef std::function< void(int, std::string)> fatal_handler_type;

  /*
  typedef ::iow::io::outgoing_handler_t outgoing_handler_type;
  typedef ::iow::io::incoming_handler_t incoming_handler_type;
  typedef ::iow::io::startup_handler_t  startup_handler_type;
  typedef ::iow::io::shutdown_handler_t shutdown_handler_type;
  */

  outgoing_handler_type outgoing_handler;
  incoming_handler_type incoming_handler;
  startup_handler_type  startup_handler;
  shutdown_handler_type shutdown_handler;
  fatal_handler_type fatal_handler;
};

}}}
