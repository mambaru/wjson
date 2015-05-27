#pragma once
//#include <functional>
#include <iow/io/basic/context.hpp>
#include <iow/io/types.hpp>

namespace iow{ namespace io{ namespace descriptor{

template<typename ID, typename DataType, typename DataPtr>
struct context
{
  typedef ::iow::io::io_id_t   io_id_type;
  typedef ::iow::io::data_type data_type;
  typedef ::iow::io::data_ptr  data_ptr;

  typedef ::iow::io::outgoing_handler_t outgoing_handler_type;
  typedef ::iow::io::incoming_handler_t incoming_handler_type;
  typedef ::iow::io::startup_handler_t  startup_handler_type;
  typedef ::iow::io::shutdown_handler_t shutdown_handler_type;

  outgoing_handler_type outgoing_handler;
  incoming_handler_type incoming_handler;
  startup_handler_type  startup_handler;
  shutdown_handler_type shutdown_handler;
};

}}}
