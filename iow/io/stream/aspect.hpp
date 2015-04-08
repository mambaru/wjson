#pragma once

#include <iow/io/stream/ad_clear.hpp>
#include <iow/io/stream/ad_initialize.hpp>
#include <iow/io/stream/ad_read_confirm.hpp>
#include <iow/io/stream/ad_read_handler.hpp>
#include <iow/io/stream/ad_read_next.hpp>
#include <iow/io/stream/ad_write_attach.hpp>
#include <iow/io/stream/ad_write_next.hpp>
#include <iow/io/stream/ad_write_confirm.hpp>
#include <iow/io/stream/tags.hpp>

#include <iow/io/pipe/aspect.hpp>
#include <iow/io/flow/aspect.hpp>
#include <iow/io/aux/write_buffer.hpp>
#include <iow/io/aux/read_buffer.hpp>
#include <iow/io/aux/data_pool.hpp>

#include <iow/io/basic/tags.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace stream{
  
template<typename DataType = std::vector<char> >
struct aspect: fas::aspect<
  ::iow::io::flow::aspect::advice_list,
  ::iow::io::pipe::aspect::advice_list,
  fas::advice< _initialize_, ad_initialize>,
  fas::advice< _clear_, ad_clear>,
  fas::group< ::iow::io::_initialize_, _initialize_>,
  fas::group< ::iow::io::_after_stop_, _clear_>,
  fas::group< ::iow::io::_after_reset_, _clear_>,
  fas::advice< ::iow::io::flow::_next_, ad_read_next >,
  fas::advice< ::iow::io::pipe::_attach_, ad_write_attach>,
  fas::advice< ::iow::io::pipe::_next_, ad_write_next>,
  fas::advice< ::iow::io::flow::_confirm_, ad_read_confirm>,
  fas::advice< ::iow::io::flow::_handler_,  ad_read_handler>,
  fas::advice< ::iow::io::pipe::_confirm_, ad_write_confirm>,
  fas::value< _write_buffer_, ::iow::io::write_buffer< DataType > >,
  fas::value< _read_buffer_, ::iow::io::read_buffer< DataType > >
  /*,
  fas::value< _buffer_pool_, std::shared_ptr< ::iow::io::data_pool<DataType> > >
  */
>{};

}}}
