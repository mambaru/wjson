#pragma once

#include <iow/ip/tcp/connection/aspect.hpp>
#include <iow/ip/tcp/client/options.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace client{

struct aspect : fas::aspect<
  fas::type< ::iow::io::_options_type_, options >,
  ::iow::ip::tcp::connection::aspect::advice_list
>{};

 
}}}}
