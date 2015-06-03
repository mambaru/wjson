#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
 
// rename peeper, follower
template<typename I>
struct provider
  : fas::value<_provider_, std::shared_ptr<I> >
{
};

}} // iow


