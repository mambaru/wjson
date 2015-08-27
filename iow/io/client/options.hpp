#pragma once

#include <string>
#include <iow/system.hpp>

namespace iow{ namespace io{ namespace client{

template<typename ConnectionOptions>
struct options
{
  ConnectionOptions connection;
  time_t reconnect_timeout_ms;
  std::string addr;
  std::string port;
  int threads;
  
  std::function<void()> connect_handler;
  std::function<void( ::iow::system::error_code )> error_handler;
  
};
  
}}}
