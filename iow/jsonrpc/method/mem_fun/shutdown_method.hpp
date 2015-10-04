#pragma once

#include <iow/jsonrpc/handler/aspect/shutdown.hpp>
#include <iow/jsonrpc/method/mem_fun/mem_fun_shutdown.hpp>
#include <memory>


namespace iow{ namespace jsonrpc{

template<
  typename Target, 
  void (Target::*mem_ptr)( io_id_t ) 
>
struct shutdown_method: shutdown< mem_fun_shutdown<Target, mem_ptr> > {};

}} // iow

