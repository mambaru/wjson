#pragma once

#include <iow/io/strand/options.hpp>

namespace iow{ namespace io{ namespace strand{

struct mtoptions : options
{
  size_t strands = 1;
  size_t threads = 1;
};

}}}
