#pragma once

#include <string>
#include <iow/system.hpp>
#include <iow/workflow/workflow.hpp>

namespace iow{ namespace io{ namespace client{

template<typename ConnectionOptions>
struct options
{
  ConnectionOptions connection;
  time_t reconnect_timeout_ms;
  size_t wait_maxsize = 0;
  size_t wait_wrnsize = 0;
  std::string addr;
  std::string port;
  int threads = 0;
  int connect_count = 1;
  std::shared_ptr< ::iow::workflow > workflow;
  std::function<void()> connect_handler;
  std::function<void( ::iow::system::error_code )> error_handler;
  
};
  
}}}
