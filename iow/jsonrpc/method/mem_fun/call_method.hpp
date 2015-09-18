#pragma once

#include <iow/jsonrpc/method/aspect/call.hpp>
#include <iow/jsonrpc/method/aspect/name.hpp>
#include <iow/jsonrpc/method/method.hpp>
#include <memory>
#include <functional>

namespace iow{ namespace jsonrpc{

template<
  typename TgName, 
  typename JParams, 
  typename JResult,
  typename ...Args
>
struct basic_call_method: basic_method< 
  name<TgName>,
  call<JParams, JResult>,
  Args...
>
{};


template<
  typename TgName, 
  typename JParams, 
  typename JResult, 
  typename ...Args
>
struct call_method: method< 
  name<TgName>,
  call<JParams, JResult>,
  Args...
>
{};

}} // iow

