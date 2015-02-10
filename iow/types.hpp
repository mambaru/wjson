#pragma once

#include <vector>
#include <memory>
#include <functional>
//#include <wfc/callback/callback_status.hpp>
#include <atomic>

namespace wfc{ namespace io{ 

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef size_t io_id_t;
typedef std::function< void(data_ptr) > outgoing_handler_t;
typedef std::function< void(data_ptr, io_id_t, outgoing_handler_t )> incoming_handler_t;


inline 
std::function<void(data_ptr, io_id_t, outgoing_handler_t  )> 
simple_handler( std::function<void(data_ptr, outgoing_handler_t )> handler )
{
  return [handler]( data_ptr d, io_id_t, outgoing_handler_t clb  )
  {
    handler( std::move(d), clb);
  };
}

inline io_id_t create_id()
{
  static std::atomic<io_id_t> counter(0);
  return ++counter;
}

typedef std::function< void(io_id_t) > shutdown_handler_t;
typedef std::function< void(shutdown_handler_t) > add_shutdown_handler_t;
typedef std::function< void(io_id_t, outgoing_handler_t, add_shutdown_handler_t) > startup_handler_t;
  
}}
