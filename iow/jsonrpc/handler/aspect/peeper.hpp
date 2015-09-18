#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{
 
/*
 * Отслеживает открытие и закрытие источников
 */
template<typename I>
struct peeper
  : fas::value<_peeper_, std::shared_ptr<I> >
{
};

}} // iow


