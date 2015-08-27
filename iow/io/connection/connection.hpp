#pragma once

#include <iow/ip/tcp/connection/aspect.hpp>
#include <iow/io/descriptor/holder.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace connection{

template<typename A = fas::aspect<> >
using connection_base = ::iow::io::descriptor::holder_base< typename fas::merge_aspect< A, aspect>::type >;

template<typename A = fas::aspect<> >
using connection = ::iow::io::descriptor::holder< typename fas::merge_aspect< A, aspect>::type >;

}}}
