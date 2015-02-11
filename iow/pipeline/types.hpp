#pragma once

#include <iow/basic/io_id.hpp>
#include <memory>
#include <atomic>
#include <functional>

namespace iow{ 

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef std::function< void(data_ptr) > outgoing_handler_t;
typedef std::function< void(io_id_t, data_ptr, outgoing_handler_t )> incoming_handler_t;

typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
typedef std::function< void(io_id_t) > shutdown_handler_t;
  
}
