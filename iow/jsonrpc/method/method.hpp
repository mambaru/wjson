#pragma once

#include <iow/jsonrpc/method/basic_method.hpp>
#include <iow/jsonrpc/method/method_group.hpp>

#include <fas/aop.hpp>
#include <fas/type_list.hpp>

namespace iow{ namespace jsonrpc{

template< typename... Args>
using method = method_group< basic_method< fas::aspect<Args...> > >;

}}
