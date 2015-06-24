#pragma once

#include <iow/ip/tcp/client/aspect.hpp>
#include <iow/io/descriptor/holder.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

template<typename A = fas::aspect<> >
using connection = ::iow::io::descriptor::holder<
  typename fas::merge_aspect<
    A,
    aspect
  >::type
>;
  
}}}}
