#pragma once

#include <iow/io/pipe/aspect.hpp>
#include <iow/io/flow/aspect.hpp>
#include <iow/io/aux/write_buffer.hpp>
#include <iow/io/aux/read_buffer.hpp>
#include <iow/io/aux/data_pool.hpp>
#include <iow/io/stream/tags.hpp>
#include <iow/io/basic/tags.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace stream{
  
template<typename DataType>
struct ad_create
{
  template<typename T>
  std::pair<char*, size_t> operator()(T& t)
  {
    /*
    std::unique_ptr<DataType> d;
    if ( nullptr != t.get_aspect().template get<_buffer_pool_>() )
    {
      d = t.get_aspect().template get<_buffer_pool_>()->create();
      std::cout << "ad_create: " << d->size() << std::endl;
    }
    else
    {
      d = std::make_unique<DataType>(4096);
    }
    t.get_aspect().template get<_read_buffer_>().attach( std::move(d) );
    */
    return t.get_aspect().template get<_read_buffer_>().next();
  }
};

struct ad_attach
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    auto &buf = t.get_aspect().template get<_write_buffer_>();
    //if ( buf.check(*d) )
    // TODO: проверить на maxbuf
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

struct ad_write_confirm
{
  template<typename T>
  void operator()(T& t, std::pair<const char*, size_t> p)
  {
    bool result = t.get_aspect().template get<_write_buffer_>().confirm(p);
    if ( !result )
    {
      // Все закртыть, и выдать в log
    }
    /*
    auto d = t.get_aspect().template get<_write_buffer_>().confirm(p);
    if ( d != nullptr )
    {
      if ( t.get_aspect().template get<_buffer_pool_>()!=nullptr )
      {
        t.get_aspect().template get<_buffer_pool_>()->free( std::move(d) );
      }
    }
    */
  }
};

struct ad_read_handler
{
  template<typename T>
  void operator()(T& t)
  {
    while (auto d = t.get_aspect().template get<_read_buffer_>().detach() )
    {
      if ( d->empty() )
      {
        std::cout << "d=[" << std::string(d->begin(), d->end() ) << "]: " << d->size() << std::endl;
        std::cout << "count: " << t.get_aspect().template get<_read_buffer_>().count()  << std::endl;
      }
      t.get_aspect().template get<_handler_>()(t, std::move(d) );
    }
  }
};

struct ad_read_confirm
{
  template<typename T>
  void operator()(T& t, std::pair<char*, size_t> p)
  {
    t.get_aspect().template get<_read_buffer_>().confirm(p);
    
    /*
    auto d = t.get_aspect().template get<_write_buffer_>().confirm(p);
    if ( d != nullptr )
    {
      if ( t.get_aspect().template get<_buffer_pool_>()!=nullptr )
      {
        t.get_aspect().template get<_buffer_pool_>()->free( std::move(d) );
      }
    }
    */
  }
};


struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O opt)
  {
    t.get_aspect().template get<_buffer_pool_>() = opt.buffer_pool;
    t.get_aspect().template get<_write_buffer_>().set_options(opt.write_buffer);
    t.get_aspect().template get<_read_buffer_>().set_options(opt.read_buffer);
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
  fas::advice< ::iow::io::flow::_confirm_, ad_read_confirm>,
  fas::advice< ::iow::io::flow::_handler_,  ad_read_handler>,
  fas::advice< ::iow::io::pipe::_confirm_, ad_write_confirm>,
  fas::value< _write_buffer_, ::iow::io::write_buffer< DataType > >,
  fas::value< _read_buffer_, ::iow::io::read_buffer< DataType > >,
  fas::value< _buffer_pool_, std::shared_ptr< ::iow::io::data_pool<DataType> > >
>{};

}}}
