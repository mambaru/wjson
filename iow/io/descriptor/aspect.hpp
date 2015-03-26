#pragma once

#include <iow/io/basic/aspect.hpp>
#include <iow/io/pipe/ad_output.hpp>
#include <iow/io/pipe/ad_more.hpp>
#include <iow/io/pipe/ad_complete.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace descriptor{

using ::iow::io::basic::aspect;
  /*
template<typename DataType>
struct context
{
  typedef DataType data_type;
  typedef DataPtr  data_ptr;
  
  typedef std::function< void(data_ptr) > outgoing_handler_t;
  typedef std::function< void(io_id_t, data_ptr, outgoing_handler_t) > incoming_handler_t;
  typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
  typedef std::function< void(io_id_t) > shutdown_handler_t;
};
  
template<typename DataType>
struct aspect: fas::aspect<
  fas::type<_context_, context<DataType> >
>{};
*/

}}}
