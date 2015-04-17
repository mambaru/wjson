#pragma once

#include <iow/io/stream/stream_options.hpp>
#include <iow/io/stream/tags.hpp>
#include <iow/io/reader/stream/tags.hpp>
#include <iow/io/reader/tags.hpp>
#include <iow/io/writer/tags.hpp>
#include <iow/io/descriptor/stream/tags.hpp>
#include <iow/io/descriptor/context.hpp>
#include <iow/io/descriptor/options.hpp>
#include <iow/io/descriptor/aspect.hpp>
#include <iow/io/descriptor/ad_initialize.hpp>
#include <iow/io/descriptor/ad_incoming_handler.hpp>
#include <iow/io/aux/data_pool.hpp>
#include <iow/system.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace descriptor{ namespace stream{

/*
typedef ::iow::io::descriptor::context<
  size_t, 
  std::vector<char>, 
  std::unique_ptr<std::vector<char> >
> stream_context;
*/
using stream_context = ::iow::io::descriptor::context<
  size_t, 
  std::vector<char>, 
  std::unique_ptr<std::vector<char> >
>;

struct context: stream_context
{
  typedef ::iow::io::data_pool< stream_context::data_type> pool_type;
  typedef std::shared_ptr< pool_type> pool_ptr;
  pool_ptr pool;
};

struct options: 
  ::iow::io::descriptor::options<context>,
  ::iow::io::stream::options<context::data_type>
{
  typedef context::pool_ptr pool_ptr;
  pool_ptr pool;
};

struct ad_after_start
{
  // TODO: startup_handler
};

struct ad_before_stop
{
  // TODO: shutdown_handler
};

struct ad_async_read_some
{
  template<typename T, typename P, typename H>
  void operator()(T& t, P p, H&& handler)
  {
    t.descriptor().async_read_some(
      ::iow::asio::buffer( p.first, p.second ),
      std::forward<H>(handler)
    );
  }
};

struct ad_async_write_some
{
  template<typename T, typename P, typename H>
  void operator()(T& t, P p, H&& handler)
  {
    t.descriptor().async_write_some(
      ::boost::asio::buffer( p.first, p.second ),
      std::forward<H>(handler)
    );
  }
};
  
struct aspect: fas::aspect<
  fas::value< _context_, context >,
  fas::advice< ::iow::io::reader::stream::_incoming_, ad_incoming_handler>,
  fas::alias< _outgoing_, ::iow::io::writer::_output_>,
  fas::advice< ::iow::io::reader::_read_some_, ad_async_read_some>,
  fas::advice< ::iow::io::writer::_write_some_, ad_async_write_some>,
  fas::advice< _initialize_, ad_initialize<true, ::iow::io::stream::_initialize_> >,
  fas::group< ::iow::io::_initialize_, _initialize_>,
  ::iow::io::descriptor::aspect
  
>{};
  
}}}}
