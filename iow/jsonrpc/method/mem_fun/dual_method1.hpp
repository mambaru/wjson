#pragma once

#include <iow/jsonrpc/method/aspect/call.hpp>
#include <iow/jsonrpc/method/aspect/name.hpp>
#include <iow/jsonrpc/method/mem_fun/invoke_mem_fun1.hpp>
#include <iow/jsonrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace iow{ namespace jsonrpc{

template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t, 
    std::weak_ptr<Target>
  ),
  typename ...Args
>
struct basic_dual_method1: basic_method< 
  name<TgName>,
  invoke_mem_fun1< JParams, JResult, Target, Target, mem_ptr>,
  call<JParams, JResult>
>
{};


template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<typename JParams::target>, 
    std::function< void(std::unique_ptr<typename JResult::target>) >, 
    size_t, 
    std::weak_ptr<Target>
  ) ,
  typename ...Args
>
struct dual_method1: method< 
  name<TgName>,
  invoke_mem_fun1< JParams, JResult, Target, Target, mem_ptr>,
  call<JParams, JResult>
>
{};


}} // iow


