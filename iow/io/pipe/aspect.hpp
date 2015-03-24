#pragma once

#include <iow/io/pipe/tags.hpp>
#include <iow/io/pipe/ad_output.hpp>
#include <iow/io/pipe/ad_more.hpp>
#include <iow/io/pipe/ad_complete.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace pipe{
  

struct aspect: fas::aspect<
  fas::advice< _output_, ad_output >,
  fas::advice< _more_, ad_more >,
  fas::advice< _complete_, ad_complete >
>{};

}}}
