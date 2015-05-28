#pragma once

#include <iow/jsonrpc/method/basic_method.hpp>
#include <iow/jsonrpc/method/method_group.hpp>
#include <fas/aop.hpp>
#include <fas/type_list.hpp>

namespace iow{ namespace jsonrpc{
  

template< typename... Args>
using method = method_group< basic_method< fas::aspect<Args...> > >;

  /*
template< typename... Args>
struct method
  : fas::type_list< 
      basic_method< fas::aspect<Args...> >, fas::type_list< 
      fas::group<_method_, typename basic_method< fas::aspect<Args...> >::tag>
    > > 
{};
*/


}}
