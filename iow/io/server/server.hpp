#pragma once

#include <fas/aop.hpp>
#include <iow/io/basic/aspect.hpp>
#include <ucommon/socket.h>

namespace iow{ namespace io{ namespace server{
  
// server{ stream{ manager??? } } или в acceptors или manager??? (descriptor::manager???? - да!!!)
struct options
{
  std::string host;
  std::string port;
  int threads;
  // TODO: incoming_handler;
};

struct ad_start_server
{
  template<typename T, typename O>
  void operator()(T& t, const O& opt)
  {
    
  }
};


struct aspect: fas::aspect<
  ::iow::io::basic::aspect<>::advice_list
>{};



  
}}}
