#pragma once

#include <string>
#include <iow/system.hpp>
#include <iow/workflow/workflow.hpp>

namespace iow{ namespace io{ namespace client{

template<typename ConnectionOptions>
struct options
{
  ConnectionOptions connection;
  bool async_connect = true;
  time_t reconnect_timeout_ms;
  std::string addr;
  std::string port;
  int threads = 0;
  int connect_count = 1;
  struct {
    std::shared_ptr< ::iow::workflow > workflow;
    std::function<void()> connect_handler;
    std::function<void( ::iow::system::error_code )> error_handler;
  } args;
  
};
  
}}}
