#pragma once

#include <iow/jsonrpc/handler/aspect/startup.hpp>
#include <iow/jsonrpc/method/mem_fun/mem_fun_startup.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{

template<
  typename Interface,
  typename Target, 
  void (Target::*mem_ptr)( ::iow::io::io_id_t, std::weak_ptr<Interface> ) 
>
struct startup_method: startup< mem_fun_startup<Interface, Target, mem_ptr> > {};

}} // iow

