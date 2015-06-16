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

#include <iow/io/basic/aspect.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace jsonrpc{
  
//struct _initialize_;
struct ad_initialize
{
  template<typename T, typename Opt>
  void operator()(T& t, const Opt& opt)
  {
    t._send_notify = opt.send_notify;
    t._send_request = opt.send_request;
    t.get_aspect().template get<_target_>() = opt.target;
    t.get_aspect().template get<_provider_>() = opt.provider;
  }
};

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
  fas::type<_handler_types_, handler_types>,
  fas::advice<_send_request_, ad_send_request>,
  fas::advice<_send_notify_, ad_send_notify>,
  fas::advice<_invoke_, ad_invoke>,
  fas::advice< ::iow::io::_initialize_, ad_initialize>,
  fas::advice< ::iow::io::_options_type_, handler_base_options>,
  ::iow::io::basic::aspect<std::recursive_mutex>::advice_list
>{};


}} // iow


