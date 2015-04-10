#pragma once

#include <iow/io/writer/ad_output.hpp>
#include <iow/io/writer/ad_more.hpp>
#include <iow/io/writer/ad_complete.hpp>
#include <iow/io/writer/ad_some.hpp>
#include <iow/io/writer/tags.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace writer{
  
struct aspect: fas::aspect<
  fas::advice< _output_, ad_output >,
  fas::advice< _more_, ad_more >,
  fas::advice< _complete_, ad_complete >,
  fas::advice< _some_, ad_some >
>{};

}}}
