#pragma once

#include <string>

namespace iow{ namespace io{ namespace acceptor{

template<typename ConnectionsOptions>
struct options
{
  typedef ConnectionsOptions connection_options_type;
  connection_options_type connection;
  
  int max_connections = 0;
  int backlog = 1024;
  std::string addr;
  std::string port;
};

  
}}}
