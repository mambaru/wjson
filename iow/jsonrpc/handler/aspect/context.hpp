#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
  
template<typename T>
struct context
  : fas::value<_context_, T >
{
};

}} // iow


