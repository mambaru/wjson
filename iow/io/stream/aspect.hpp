#pragma once

#include <iow/io/pipe/aspect.hpp>
#include <iow/io/flow/aspect.hpp>
#include <iow/io/aux/write_buffer.hpp>
#include <iow/io/aux/data_pool.hpp>
#include <iow/io/stream/tags.hpp>
#include <iow/io/basic/tags.hpp>
#include <fas/aop.hpp>
#include <../cdaemon/geoip_converter/types.h>

namespace iow{ namespace io{ namespace stream{
  
template<typename DataType>
struct ad_create
{
  template<typename T>
  std::unique_ptr<DataType> operator()(T& t)
  {
    if ( nullptr != t.get_aspect().template get<_buffer_pool_>() )
    {
      return t.get_aspect().template get<_buffer_pool_>()->create();
    }
    else
    {
      return std::make_unique<DataType>(4096);
    }
  }
};

struct ad_attach
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    auto &buf = t.get_aspect().template get<_write_buffer_>();
    if ( buf.check(*d) )
      buf.attach(std::move(d));
  }
};

struct ad_next
{
  template<typename T>
  std::pair<const char*, size_t> operator()(T& t)
  {
    return t.get_aspect().template get<_write_buffer_>().next();
  }
};

struct ad_confirm
{
  template<typename T>
  void operator()(T& t, std::pair<const char*, size_t> p)
  {
    auto d = t.get_aspect().template get<_write_buffer_>().confirm(p);
    if ( d != nullptr )
    {
      if ( t.get_aspect().template get<_buffer_pool_>()!=nullptr )
      {
        t.get_aspect().template get<_buffer_pool_>()->free( std::move(d) );
      }
    }
  }
};

struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O opt)
  {
    t.get_aspect().template get<_buffer_pool_>() = opt.buffer_pool;
    t.get_aspect().template get<_write_buffer_>().set_options(opt.write_buffer);
    std::cout << std::endl << "ad_initialize" << std::endl;
  }
};

struct ad_clear
{
  template<typename T>
  void operator()(T& t )
  {
    t.get_aspect().template get<_write_buffer_>().clear();
  }
};

template<typename DataType = std::vector<char> >
struct aspect: fas::aspect<
  ::iow::io::flow::aspect::advice_list,
  ::iow::io::pipe::aspect::advice_list,
  fas::advice< _initialize_, ad_initialize>,
  fas::advice< _clear_, ad_clear>,
  fas::group< ::iow::io::_initialize_, _initialize_>,
  fas::group< ::iow::io::_after_stop_, _clear_>,
  fas::group< ::iow::io::_after_reset_, _clear_>,
  fas::advice< ::iow::io::flow::_create_, ad_create< DataType > >,
  fas::advice< ::iow::io::pipe::_attach_, ad_attach>,
  fas::advice< ::iow::io::pipe::_next_, ad_next>,
  fas::advice< ::iow::io::pipe::_confirm_, ad_confirm>,
  fas::value< _write_buffer_, ::iow::io::write_buffer< DataType > >,
  fas::value< _buffer_pool_, std::shared_ptr< ::iow::io::data_pool<DataType> > >
>{};

}}}
