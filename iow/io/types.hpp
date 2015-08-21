#pragma once

#include <vector>
#include <functional>
#include <memory>

namespace iow{ namespace io{

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef size_t io_id_t;

typedef std::function< void(data_ptr) > outgoing_handler_t;
typedef std::function< void(data_ptr, io_id_t, outgoing_handler_t )> incoming_handler_t;
typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
typedef std::function< void(io_id_t) > shutdown_handler_t;

}}

