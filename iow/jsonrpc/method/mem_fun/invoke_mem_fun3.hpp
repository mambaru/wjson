#pragma once

#include <iow/jsonrpc/method/aspect/invoke.hpp>
#include <iow/jsonrpc/method/mem_fun/mem_fun_handler3.hpp>

namespace iow{ namespace jsonrpc{

template<
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t
  )
>
struct invoke_mem_fun3
  : invoke<
      JParams, 
      JResult, 
      mem_fun_handler3<
        typename JParams::target, 
        typename JResult::target,
        Target, 
        mem_ptr
      > 
    >
{};

}} // iow


