#pragma once

#include <iow/io/acceptor/aspect.hpp>
#include <iow/io/descriptor/holder.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace acceptor{

template <typename ConnectionType, typename A = fas::empty_type>
using acceptor = ::iow::io::descriptor::holder< typename fas::merge_aspect<A, aspect<ConnectionType> >::type >;

}}}
