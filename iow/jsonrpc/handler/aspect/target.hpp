#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
  
template<typename I>
struct target
  : fas::value<_target_, std::shared_ptr<I> >
{
};

}} 


