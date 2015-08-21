#pragma once

#include <string>

namespace iow{ namespace io{ namespace acceptor{

template<typename ConnectionsOptions>
struct options
{
  typedef ConnectionsOptions connection_options_type;
  connection_options_type connection;
  
  int max_connections = 0;
  std::string host;
};

  
}}}
