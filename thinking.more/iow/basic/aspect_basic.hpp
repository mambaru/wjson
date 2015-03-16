#pragma once

#include <iow/basic/tags.hpp>
#include <iow/basic/io_context.hpp>
#include <fas/aop.hpp>
#include <mutex>

namespace iow{
  
struct aspect_basic: fas::aspect<
  fas::type<_options_type_, fas::empty_type>,
  fas::type<_context_type_, io_context>,
  fas::type<_mutex_type_, std::recursive_mutex>
>{};
  
}
