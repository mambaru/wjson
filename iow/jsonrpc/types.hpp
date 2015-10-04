#pragma once

#include <iow/io/types.hpp>
#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>
#include <iow/jsonrpc/basic_types.hpp>

namespace iow{ namespace jsonrpc{

typedef std::function< void(outgoing_holder) > outgoing_jsonrpc_t;
typedef std::function< void(incoming_holder, io_id_t, outgoing_jsonrpc_t )> incoming_jsonrpc_t;
typedef std::function< void(io_id_t, outgoing_jsonrpc_t) > startup_jsonrpc_t;
typedef std::function< void(io_id_t) > shutdown_jsonrpc_t;

}}

