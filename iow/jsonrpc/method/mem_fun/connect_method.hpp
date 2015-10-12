#pragma once

#include <iow/jsonrpc/handler/aspect/connect.hpp>
#include <iow/jsonrpc/method/mem_fun/mem_fun_connect.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{

template<
  typename Interface,
  typename Target, 
  void (Target::*mem_ptr)( io_id_t, std::weak_ptr<Interface> ) 
>
struct connect_method: connect< mem_fun_connect<Interface, Target, mem_ptr> > {};

}} // iow

