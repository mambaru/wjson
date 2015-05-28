#pragma once

#include <iow/jsonrpc/method/aspect/tags.hpp>
#include <iow/jsonrpc/method/aspect/name.hpp>
#include <iow/jsonrpc/method/aspect/call.hpp>
#include <iow/jsonrpc/method/aspect/invoke.hpp>

#include <iow/jsonrpc/method/aspect/invoke_stub.hpp>
#include <iow/jsonrpc/method/aspect/send_error.hpp>
#include <iow/jsonrpc/method/aspect/send_result.hpp>
#include <iow/jsonrpc/method/aspect/request_serializer.hpp>
#include <iow/jsonrpc/method/aspect/notify_serializer.hpp>
#include <iow/jsonrpc/method/aspect/process_response.hpp>
#include <iow/jsonrpc/method/aspect/tags.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace jsonrpc{
  
struct aspect_method
  : fas::aspect< 
      request_serializer_proxy,
      notify_serializer_proxy,
      process_response_proxy,
      send_error_proxy,
      send_result_proxy,
      invoke_stub
    > 
{
  
};

}} // iow


