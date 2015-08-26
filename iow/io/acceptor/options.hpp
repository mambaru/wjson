#pragma once

#include <string>
#include <iow/io/descriptor/options.hpp>
#include <iow/io/acceptor/context.hpp>

namespace iow{ namespace io{ namespace acceptor{

template<typename ConnectionType, typename ConnectionsOptions>
struct options
  : public ::iow::io::descriptor::options< context<ConnectionType> >
{
  typedef ConnectionsOptions connection_options_type;
  connection_options_type connection;
  
  int max_connections = 0;
  int backlog = 1024;
  std::string addr;
  std::string port;
};

  
}}}
