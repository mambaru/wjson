#pragma once
#include <iow/jsonrpc/types.hpp>
#include <iow/workflow/workflow.hpp>

namespace iow{ namespace jsonrpc{
  
template<typename HandlerOptions>
struct engine_options: HandlerOptions
{
  typedef HandlerOptions handler_options;
  bool allow_non_jsonrpc = false;
  // максимальное время ожидания ответа на вызов (0 - неограничено)
  time_t call_lifetime_ms   = 60000;
  time_t remove_outdated_ms = 1000;
  bool   remove_everytime = true;
  
  struct {
    std::weak_ptr< ::iow::workflow > workflow;
  } engine_args;
};

}}
