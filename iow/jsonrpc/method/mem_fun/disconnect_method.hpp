#pragma once

#include <iow/jsonrpc/handler/aspect/disconnect.hpp>
#include <iow/jsonrpc/method/mem_fun/mem_fun_disconnect.hpp>
#include <memory>


namespace iow{ namespace jsonrpc{

template<
  typename Target, 
  void (Target::*mem_ptr)( io_id_t ) 
>
struct disconnect_method: disconnect< mem_fun_disconnect<Target, mem_ptr> > {};

}} // iow

