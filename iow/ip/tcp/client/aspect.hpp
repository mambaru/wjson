#pragma once

#include <iow/ip/tcp/client/options.hpp>
#include <iow/io/basic/tags.hpp>
#include <iow/io/descriptor/tags.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

struct aspect : fas::aspect<
  fas::type< ::iow::io::_options_type_, options >/*,
  fas::type< ::iow::io::descriptor::_descriptor_type_, iow::asio::ip::tcp::client >*/
>{};
  
}}}}
