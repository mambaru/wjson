#pragma once

#include <iow/jsonrpc/handler/basic_method_list.hpp>

namespace iow{ namespace jsonrpc{

template<typename... Args >
class method_list
  : public basic_method_list<Args...>
{
public:
  typedef basic_method_list<Args...> super;
  typedef typename basic_method_list<Args...>::interface_type interface_type;
  typedef typename basic_method_list<Args...>::handler_types handler_types;
};


}} // iow


