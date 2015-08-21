#pragma once

#include <iow/ip/tcp/connection/options.hpp>

#include <iow/io/reader/asio/aspect.hpp>
#include <iow/io/writer/asio/aspect.hpp>
#include <iow/io/stream/aspect.hpp>
#include <iow/io/basic/aspect.hpp>
#include <iow/io/descriptor/tags.hpp>
#include <iow/io/connection/aspect.hpp>

#include <fas/aop.hpp>
#include <mutex>
#include <vector>

namespace iow{ namespace ip{ namespace tcp{ namespace connection{

struct aspect : fas::aspect<
    fas::type< ::iow::io::descriptor::_descriptor_type_, ::iow::asio::ip::tcp::socket >,
    fas::type< ::iow::io::_options_type_, options >,
    // Все это должно быть в ::iow::io::descriptor::stream::aspect
    ::iow::io::connection::aspect,
    //::iow::io::descriptor::stream::aspect,
    ::iow::io::reader::asio::aspect,
    ::iow::io::writer::asio::aspect,
    ::iow::io::stream::aspect< std::vector<char> >,
    ::iow::io::basic::aspect< std::recursive_mutex >::advice_list
>{};
  
}}}}
