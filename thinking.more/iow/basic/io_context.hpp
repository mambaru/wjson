#pragma once

#include <iow/basic/types.hpp>

namespace iow{ 

struct io_context
{
  io_id_t id;
  bool status;
  
  void reset()
  {
    id = 0;
    status = 0;
  }
};

}