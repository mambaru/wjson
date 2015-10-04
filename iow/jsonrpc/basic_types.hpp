#pragma once

#include <iow/io/types.hpp>

namespace iow{ namespace jsonrpc{

typedef ::iow::io::io_id_t   io_id_t;
typedef ::iow::io::data_type data_type;
typedef ::iow::io::data_ptr  data_ptr;

typedef data_ptr incoming_call_id_t;
typedef int      call_id_t;
typedef int      error_code_t;

// typedef ::iow::io::outgoing_handler_t outgoing_handler_t;
// typedef ::iow::io::incoming_handler_t incoming_handler_t;
// typedef ::iow::io::startup_handler_t  startup_handler_t;
// typedef ::iow::io::shutdown_handler_t shutdown_handler_t;

}}

