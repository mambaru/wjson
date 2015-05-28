#pragma once

#include <iow/jsonrpc/method/aspect/name.hpp>
#include <iow/jsonrpc/method/mem_fun/invoke_mem_fun2.hpp>
#include <iow/jsonrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace iow{ namespace jsonrpc{

template<
  typename TgName, 
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
  ),
  typename ...Args
>
struct basic_invoke_method2: basic_method< 
  name<TgName>,
  invoke_mem_fun2< 
    JParams, 
    JResult, 
    Params2, 
    Result2, 
    Target, 
    mem_ptr, 
    Itf, 
    mem_ptr2
  >,
  Args...
>
{};

  
template<
  typename TgName, 
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
  ),
  typename ...Args
>
struct invoke_method2: method< 
  name<TgName>,
  invoke_mem_fun2< 
    JParams, 
    JResult, 
    Params2, 
    Result2, 
    Target, 
    mem_ptr, 
    Itf, 
    mem_ptr2
  >,
  Args...
>
{};


}} // iow


