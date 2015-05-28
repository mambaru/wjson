#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop/advice.hpp>

namespace iow{ namespace jsonrpc{

template<typename Handler>
struct startup:
  fas::advice<_startup_, Handler>
{};


}} // iow


