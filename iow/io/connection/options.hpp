#pragma once

#include <iow/io/connection/context.hpp>
#include <iow/io/descriptor/options.hpp>
#include <iow/io/stream/options.hpp>

namespace iow{ namespace io{ namespace connection{

struct options: 
  ::iow::io::descriptor::options<context>,
  ::iow::io::stream::options<context::data_type>
{
  /*typedef context::pool_ptr pool_ptr;
  pool_ptr pool;*/
};

  
}}}
