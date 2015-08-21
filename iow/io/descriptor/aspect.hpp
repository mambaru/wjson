#pragma once

#include <iow/io/basic/aspect.hpp>
#include <iow/io/descriptor/tags.hpp>
#include <iow/io/descriptor/ad_before_stop.hpp>
#include <iow/io/descriptor/ad_after_start.hpp>
#include <iow/io/descriptor/ad_incoming_handler.hpp>
#include <iow/io/descriptor/context.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace descriptor{

template<typename ContextType >
struct aspect: fas::aspect<
  fas::value< _context_, ContextType >,
  fas::advice< _before_stop_, ad_before_stop>,
  fas::advice< _after_start_, ad_after_start>,
  fas::group< ::iow::io::_after_start_, _after_start_>,
  fas::group< ::iow::io::_before_stop_, _before_stop_>,
  fas::advice<_incoming_, ad_incoming_handler>
> {};

}}}
