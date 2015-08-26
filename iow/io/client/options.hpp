#pragma once

#include <string>

namespace iow{ namespace io{ namespace client{

template<typename ConnectionOptions>
struct options
{
  ConnectionOptions connection;
  std::string addr;
  std::string port;
  int threads;
};
  
}}}
