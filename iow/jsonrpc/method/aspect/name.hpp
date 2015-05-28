#pragma once

#include <iow/jsonrpc/method/aspect/tags.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace jsonrpc{
  
template<typename N>
struct name
{
  typedef fas::metalist::advice metatype;
  typedef _name_ tag;
  typedef name<N> advice_class;
  typedef N name_type;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}

  const char* operator()() const
  {
    return name_type()();
  }
};

}} // iow


