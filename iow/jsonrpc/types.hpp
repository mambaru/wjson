#pragma once

#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>
#include <iow/jsonrpc/basic_types.hpp>

namespace iow{ namespace jsonrpc{

typedef std::function< void(outgoing_holder) > outgoing_handler_t;
typedef std::function< void(incoming_holder, io_id_t, outgoing_handler_t )> incoming_handler_t;
typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
typedef std::function< void(io_id_t) > shutdown_handler_t;

}}

