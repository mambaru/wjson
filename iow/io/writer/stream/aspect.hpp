#pragma once

#include <iow/io/writer/stream/ad_clear.hpp>
#include <iow/io/writer/stream/ad_initialize.hpp>
#include <iow/io/writer/stream/ad_attach.hpp>
#include <iow/io/writer/stream/ad_next.hpp>
#include <iow/io/writer/stream/ad_confirm.hpp>
#include <iow/io/writer/stream/tags.hpp>
#include <iow/io/writer/aspect.hpp>
#include <iow/io/aux/write_buffer.hpp>
#include <iow/io/basic/tags.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace writer{ namespace stream{
  
template<typename DataType = std::vector<char> >
struct aspect: fas::aspect<
  
  ::iow::io::writer::aspect::advice_list,
  fas::advice< _initialize_, ad_initialize>,
  fas::advice< _clear_, ad_clear>,
  fas::group< ::iow::io::_after_stop_, _clear_>,
  fas::group< ::iow::io::_after_reset_, _clear_>,
  fas::advice< ::iow::io::writer::_next_, ad_next >,
  fas::advice< ::iow::io::writer::_confirm_, ad_confirm>,
  fas::advice< ::iow::io::writer::_attach_,  ad_attach>,
  fas::value< _write_buffer_, ::iow::io::write_buffer< DataType > >

  /*
  typename ::iow::io::reader::stream::aspect<DataType>::advice_list,
  ::iow::io::writer::aspect::advice_list,
  fas::advice< _initialize_, ad_initialize>,
  fas::advice< _clear_, ad_clear>,
  fas::group< ::iow::io::_after_stop_, _clear_>,
  fas::group< ::iow::io::_after_reset_, _clear_>,
  fas::advice< ::iow::io::writer::_attach_, ad_write_attach>,
  fas::advice< ::iow::io::writer::_next_, ad_write_next>,
  fas::advice< ::iow::io::writer::_confirm_, ad_write_confirm>,
  fas::value< _write_buffer_, ::iow::io::write_buffer< DataType > >
  */
>{};

}}}}
