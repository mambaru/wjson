#pragma once

#include <iow/ip/tcp/acceptor/aspect/ad_set_reuse_address.hpp>
#include <iow/ip/tcp/acceptor/aspect/ad_sync_resover.hpp>
#include <iow/ip/tcp/acceptor/aspect/tags.hpp>
#include <iow/ip/tcp/acceptor/options.hpp>
#include <iow/io/acceptor/tags.hpp>
#include <iow/io/basic/tags.hpp>
#include <iow/io/descriptor/tags.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace acceptor{

struct aspect : fas::aspect<
  fas::advice< _set_reuse_address_, ad_set_reuse_address>,
  fas::advice< _sync_resolver_, ad_sync_resolver>,
  fas::alias< ::iow::io::acceptor::_sync_resolver_, _sync_resolver_>,
  fas::group< ::iow::io::acceptor::_set_acceptor_options_, _set_reuse_address_>,
  fas::type< ::iow::io::_options_type_, options >,
  fas::type< ::iow::io::descriptor::_descriptor_type_, iow::asio::ip::tcp::acceptor >
>{};
  
}}}}
