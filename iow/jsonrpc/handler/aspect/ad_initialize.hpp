#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <iostream>

namespace iow{ namespace jsonrpc{
  
struct ad_initialize
{
  template<typename T, typename Opt>
  void operator()(T& t, const Opt& opt)
  {
    std::cout << "handler::ad_initialize -1-" << std::endl;
    t._send_notify = opt.send_notify;
    t._send_request = opt.send_request;
    t.get_aspect().template get<_target_>() = opt.target;
    t.get_aspect().template get<_peeper_>() = opt.peeper;
    std::cout << "handler::ad_initialize -2-" << std::endl;
  }
};

}} // iow


