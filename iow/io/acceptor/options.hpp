#pragma once

#include <string>

namespace iow{ namespace io{ namespace acceptor{

struct options
{
  std::string host;
  std::string port;
  int backlog = 1024;
};

  
}}}
