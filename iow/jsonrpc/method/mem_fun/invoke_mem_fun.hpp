#pragma once

#include <iow/jsonrpc/method/aspect/invoke.hpp>
#include <iow/jsonrpc/method/mem_fun/mem_fun_handler.hpp>

namespace iow{ namespace jsonrpc{
  
template<
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) > 
  ) 
>
struct invoke_mem_fun
  : invoke<
      JParams, 
      JResult, 
      mem_fun_handler<
        typename JParams::target, 
        typename JResult::target, 
        Target, 
        mem_ptr
      >
    >
{};

}} // iow


