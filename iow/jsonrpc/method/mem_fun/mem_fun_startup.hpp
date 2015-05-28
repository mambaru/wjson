#pragma once

#include <iow/jsonrpc/method/mem_fun/mem_fun_helper.hpp>

namespace iow{ namespace jsonrpc{

template<
  typename Interface,
  typename Target, 
  void (Target::*mem_ptr)( io::io_id_t, std::shared_ptr<Interface> ) 
>
struct mem_fun_startup
{
  template<typename T>
  void operator()(T& t, io::io_id_t id) const
  {
    if (auto trg = t.provider() )
    {
      (trg.get()->*mem_ptr)( id, t.shared_from_this() );
    }
    else
    {
    }
  }
};

}} // iow


