#pragma once

#include <iow/jsonrpc/method/aspect/invoke.hpp>
#include <iow/jsonrpc/method/mem_fun/mem_fun_handler2.hpp>

namespace iow{ namespace jsonrpc{
 
template<
  typename JParams, 
  typename JResult, 
  typename Params2, 
  typename Result2, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t, 
    std::function< void(
      std::unique_ptr<Params2>, 
      std::function< void(std::unique_ptr<Result2>) >  
    ) >
  ),
  typename Itf,
  void (Itf::*mem_ptr2)( 
    std::unique_ptr<Params2>, 
    std::function< void(std::unique_ptr<Result2>) >
  )
>
struct invoke_mem_fun2
  : invoke<
      JParams, 
      JResult, 
      mem_fun_handler2<
        typename JParams::target, 
        typename JResult::target,
        Params2,
        Result2,
        Target, 
        mem_ptr,
        Itf,
        mem_ptr2
      > 
    >
{};



}} // iow


