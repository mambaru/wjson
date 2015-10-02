#pragma once

#include <iow/jsonrpc/method/aspect/invoke.hpp>
#include <iow/jsonrpc/method/mem_fun/mem_fun_handler1.hpp>

namespace iow{ namespace jsonrpc{
  
template<
  typename JParams, 
  typename JResult, 
  typename Target, 
  typename Itf,
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t, 
    std::weak_ptr<Itf>
  ) 
>
struct invoke_mem_fun1
  : invoke<
      JParams, 
      JResult, 
      mem_fun_handler1<
        typename JParams::target, 
        typename JResult::target, 
        Target, 
        Itf, 
        mem_ptr
      > 
    >
{
  
};

}} // iow


