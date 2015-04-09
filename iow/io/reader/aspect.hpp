#pragma once

#include <iow/io/reader/tags.hpp>
#include <iow/io/reader/ad_start.hpp>
#include <iow/io/reader/ad_more.hpp>
#include <iow/io/reader/ad_complete.hpp>
#include <iow/io/reader/ad_some.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace reader{

struct aspect: fas::aspect<
  fas::advice< _start_, ad_start >,
  fas::advice< _more_, ad_more >,
  fas::advice< _complete_, ad_complete >,
  fas::advice< _some_, ad_some >,
  fas::group< ::iow::io::_after_start_, _start_>
>{};

}}}
