#pragma once

#include <iow/io/reader/asio/ad_read_handler.hpp>
#include <iow/io/reader/asio/ad_make_handler.hpp>
#include <iow/io/reader/tags.hpp>
#include <iow/io/reader/asio/tags.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace reader{ namespace asio{

struct aspect: fas::aspect<
    fas::advice<_make_handler_, ad_make_handler>,
    fas::advice<_read_handler_, ad_read_handler>
>{};


}}}}

