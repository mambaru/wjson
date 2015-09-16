#pragma once

#include <iow/io/writer/asio/ad_write_handler.hpp>
#include <iow/io/writer/asio/ad_make_handler.hpp>
#include <iow/io/writer/asio/ad_error_handler.hpp>
#include <iow/io/writer/tags.hpp>
#include <iow/io/writer/asio/tags.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace writer{ namespace asio{

struct aspect: fas::aspect<
    fas::advice<_make_handler_, ad_make_handler>,
    fas::advice<_write_handler_, ad_write_handler>,
    fas::advice<_error_handler_, ad_error_handler>
>{};


}}}}

