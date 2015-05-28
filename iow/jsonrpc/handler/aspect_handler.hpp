#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <iow/jsonrpc/handler/aspect/target.hpp>
#include <iow/jsonrpc/handler/aspect/interface_.hpp>
#include <iow/jsonrpc/handler/aspect/dual_interface.hpp>
#include <iow/jsonrpc/handler/aspect/provider.hpp>
#include <iow/jsonrpc/handler/aspect/startup.hpp>
#include <iow/jsonrpc/handler/aspect/shutdown.hpp>
#include <iow/jsonrpc/handler/aspect/context.hpp>
#include <iow/jsonrpc/handler/aspect/ad_response_handler.hpp>
#include <iow/jsonrpc/handler/aspect/ad_send_request.hpp>
#include <iow/jsonrpc/handler/aspect/ad_send_notify.hpp>
#include <iow/jsonrpc/handler/aspect/ad_invoke.hpp>
#include <iow/jsonrpc/handler/ihandler.hpp>

#include <iow/jsonrpc/method/aspect/request_serializer.hpp>
#include <iow/jsonrpc/method/aspect/notify_serializer.hpp>
#include <iow/jsonrpc/method/aspect/process_response.hpp>
#include <iow/jsonrpc/method/aspect/send_error.hpp>
#include <iow/jsonrpc/method/aspect/send_result.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace jsonrpc{

struct aspect_handler: fas::aspect<
  target<fas::empty_type>, 
  interface_<fas::empty_type>,
  provider<fas::empty_type>, 
  context<fas::empty_type>,
  fas::stub<_startup_>, 
  fas::stub<_shutdown_>,
  request_serializer<80>,
  notify_serializer<80>,
  send_error<80>,
  send_result<80>,
  process_response,
  fas::advice<_response_handler_, ad_response_handler>,
  fas::type<_ihandler_, ihandler>,
  fas::advice<_send_request_, ad_send_request>,
  fas::advice<_send_notify_, ad_send_notify>,
  fas::advice<_invoke_, ad_invoke>
>{};


}} // iow


