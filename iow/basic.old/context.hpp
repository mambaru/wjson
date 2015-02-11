#pragma once

#include <wfc/io/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace io{
  
template<typename V = fas::empty_type>
struct context: 
  ::fas::value<_context_, V>
{
};

}}
