#pragma once

#include <iow/io/basic/aspect.hpp>
#include <iow/io/io_base.hpp>
#include <fas/aop.hpp>
#include <mutex>

namespace iow{ namespace io{

template<typename A=fas::empty_type>
using basic_io = io_base< typename fas::merge_aspect<A, basic::aspect<std::recursive_mutex> >::type >;
}}
