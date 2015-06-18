#pragma once

#include <iow/io/stream/options.hpp>
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


#include <iow/io/descriptor/stream/ad_async_read_some.hpp>
#include <iow/io/descriptor/stream/ad_async_write_some.hpp>

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
  template<typename T>
  void operator()(T& t)
  {
    auto& cntx = t.get_aspect().template get< _context_ >();
    if ( cntx.startup_handler )
    {
      cntx.startup_handler( t.get_id_(t), cntx.outgoing_handler);
    }
  }
};

struct ad_before_stop
{
  // TODO: shutdown_handler
  // TODO: в дескриптор
  template<typename T>
  void operator()(T& t)
  {
    //auto& cntx_basic = t.get_aspect().template get< ::iow::io::basic::_context_ >();
    auto& cntx = t.get_aspect().template get< _context_ >();
    if ( cntx.shutdown_handler )
    {
      cntx.shutdown_handler( t.get_id_(t) );
    }
    t.descriptor().close();
  }
};


struct aspect: fas::aspect<
  fas::value< _context_, context >,
  fas::advice< _before_stop_, ad_before_stop>,
  fas::group< ::iow::io::_before_stop_, _before_stop_>,
  fas::advice< ::iow::io::reader::stream::_incoming_, ad_incoming_handler>,
  fas::alias< _outgoing_, ::iow::io::writer::_output_>,
  fas::advice< ::iow::io::reader::_read_some_, ad_async_read_some>,
  fas::advice< ::iow::io::writer::_write_some_, ad_async_write_some>,
  fas::advice< _initialize_, ad_initialize<true, ::iow::io::stream::_initialize_> >,
  fas::group< ::iow::io::_initialize_, _initialize_>,
  ::iow::io::descriptor::aspect
  
>{};
  
}}}}
