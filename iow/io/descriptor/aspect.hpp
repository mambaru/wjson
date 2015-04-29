#pragma once

#include <iow/io/basic/aspect.hpp>
#include <iow/io/descriptor/tags.hpp>
#include <iow/io/descriptor/ad_incoming_handler.hpp>
#include <iow/io/descriptor/context.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace descriptor{

  
struct aspect: fas::aspect<
  fas::advice<_incoming_, ad_incoming_handler>/*,
  fas::advice<_context_, context<> >*/
> {};

}}}
