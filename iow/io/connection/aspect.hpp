#pragma once

#include <iow/io/stream/options.hpp>
#include <iow/io/stream/tags.hpp>
#include <iow/io/reader/stream/tags.hpp>
#include <iow/io/reader/tags.hpp>
#include <iow/io/writer/tags.hpp>
#include <iow/io/descriptor/context.hpp>
#include <iow/io/descriptor/options.hpp>
#include <iow/io/descriptor/aspect.hpp>
#include <iow/io/descriptor/ad_initialize.hpp>
#include <iow/io/descriptor/ad_incoming_handler.hpp>
#include <iow/io/aux/data_pool.hpp>
#include <iow/system.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>
#include <iow/io/connection/tags.hpp>

#include <iow/io/connection/asio/aspect.hpp>
#include <iow/io/connection/context.hpp>


namespace iow{ namespace io{ namespace connection{

  /*
*/

  /*
*/


//template<typename ContextType = context>
struct aspect: fas::aspect<
  fas::advice< ::iow::io::reader::stream::_incoming_, ::iow::io::descriptor::ad_incoming_handler>,
  fas::alias< ::iow::io::descriptor::_outgoing_, ::iow::io::writer::_output_>,
  ::iow::io::connection::asio::aspect,
  /*fas::advice< ::iow::io::reader::_read_some_, ad_async_read_some>,
  fas::advice< ::iow::io::writer::_write_some_, ad_async_write_some>,
  */
  fas::advice< _initialize_, ::iow::io::descriptor::ad_initialize<true, ::iow::io::stream::_initialize_> >,
  fas::group< ::iow::io::_initialize_, _initialize_>,
  ::iow::io::descriptor::aspect< context >
>{};
  
}}}
