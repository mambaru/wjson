#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <iostream>

namespace iow{ namespace jsonrpc{

struct ad_initialize
{
  template<typename T, typename Opt>
  void operator()(T& t, const Opt& opt)
  {
    t._sender_handler = opt.sender_handler;
    t.get_aspect().template get<_target_>() = opt.target;
    t.get_aspect().template get<_peeper_>() = opt.peeper;
  }
};

}} // iow


