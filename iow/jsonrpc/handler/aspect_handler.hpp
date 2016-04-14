#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <iow/jsonrpc/handler/aspect/target.hpp>
#include <iow/jsonrpc/handler/aspect/interface_.hpp>
#include <iow/jsonrpc/handler/aspect/peeper.hpp>
#include <iow/jsonrpc/handler/aspect/connect.hpp>
#include <iow/jsonrpc/handler/aspect/disconnect.hpp>
#include <iow/jsonrpc/handler/aspect/context.hpp>
#include <iow/jsonrpc/handler/aspect/ad_response_handler.hpp>
#include <iow/jsonrpc/handler/aspect/ad_perform_send.hpp>

#include <iow/jsonrpc/handler/aspect/ad_invoke.hpp>
#include <iow/jsonrpc/handler/aspect/ad_invoke_error.hpp>
#include <iow/jsonrpc/handler/aspect/ad_initialize.hpp>
#include <iow/jsonrpc/handler/handler_base_options.hpp>

#include <iow/jsonrpc/method/aspect/request_serializer.hpp>
#include <iow/jsonrpc/method/aspect/notify_serializer.hpp>
#include <iow/jsonrpc/method/aspect/process_response.hpp>
#include <iow/jsonrpc/method/aspect/send_error.hpp>
#include <iow/jsonrpc/method/aspect/send_result.hpp>

#include <iow/io/basic/aspect.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace jsonrpc{
  
struct aspect_handler: fas::aspect<
  target<fas::empty_type>, 
  interface_<fas::empty_type>,
  peeper<fas::empty_type>, 
  context<fas::empty_type>,
  fas::stub<_connect_>, 
  fas::stub<_disconnect_>,
  request_serializer<80>,
  notify_serializer<80>,
  send_error<80>,
  send_result<80>,
  process_response,
  fas::advice<_response_handler_, ad_response_handler>,
  fas::type<_handler_types_, handler_types>,
  
  fas::advice<_perform_send_, ad_perform_send>,
  //fas::advice<_send_request_, ad_send_request>,
  //fas::advice<_send_notify_, ad_send_notify>,
  fas::advice<_invoke_, ad_invoke>,
  fas::advice<_invoke_error_, ad_invoke_error>,
  fas::advice< ::iow::io::_initialize_, ad_initialize>,
  fas::advice< ::iow::io::_options_type_, handler_base_options>,
  ::iow::io::basic::aspect<std::recursive_mutex>::advice_list
>{};


}} // iow


