#pragma once
#include <string>

namespace iow{ namespace io{ namespace strand{

struct options
{
  bool disabled = false;
  size_t maxsize = 0;
  size_t wrnsize = 0;
};

}}}
