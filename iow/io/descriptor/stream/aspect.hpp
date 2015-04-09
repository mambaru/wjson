#pragma once

#include <iow/io/stream/stream_options.hpp>
#include <iow/io/stream/tags.hpp>
#include <iow/io/reader/stream/tags.hpp>
#include <iow/io/reader/tags.hpp>
#include <iow/io/writer/tags.hpp>
#include <iow/io/descriptor/stream/tags.hpp>
#include <iow/io/aux/data_pool.hpp>
#include <iow/system.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace descriptor{ namespace stream{

struct context
{
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type>  data_ptr;
  typedef ::iow::io::data_pool<data_type> pool_type;
  typedef std::shared_ptr<pool_type> pool_ptr;
  typedef size_t io_id_t;
  typedef std::function< void(data_ptr) > outgoing_handler_fun;
  typedef std::function< void(data_ptr, io_id_t, outgoing_handler_fun) > incoming_handler_fun;
  typedef std::function< void(io_id_t, outgoing_handler_fun) > startup_handler_fun;
  typedef std::function< void(io_id_t) > shutdown_handler_fun;

  outgoing_handler_fun outgoing_handler;
  incoming_handler_fun incoming_handler;
  startup_handler_fun  startup_handler;
  shutdown_handler_fun shutdown_handler;
  
};

struct options: 
  public ::iow::io::stream::stream_options<context::data_type>
{
  typedef context::data_type data_type;
  typedef context::data_ptr  data_ptr; 
  typedef context::pool_ptr              pool_ptr;
  typedef context::outgoing_handler_fun  outgoing_handler_fun;
  typedef context::incoming_handler_fun  incoming_handler_fun;
  typedef context::startup_handler_fun   startup_handler_fun;
  typedef context::shutdown_handler_fun  shutdown_handler_fun;
  
  outgoing_handler_fun outgoing_handler;
  incoming_handler_fun incoming_handler;
  startup_handler_fun  startup_handler;
  shutdown_handler_fun shutdown_handler;
  pool_ptr pool;
};

// Универсальный для всех
struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O&& opt) 
  {
    std::cout << "ad_initialize" << std::endl;
    auto& cntx = t.get_aspect().template get<_context_>();
    cntx.outgoing_handler  = opt.outgoing_handler;
    cntx.incoming_handler  = opt.incoming_handler;
    cntx.startup_handler   = opt.startup_handler;
    cntx.shutdown_handler  = opt.shutdown_handler;
    t.get_aspect().template get< ::iow::io::stream::_initialize_>()( t, std::forward<O>(opt) );
    // TODO: Все осталдьное
  }
};


struct ad_read_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, ::iow::system::error_code ec , std::size_t bytes_transferred)
  {
    std::cout << "ad_read_handler1" << std::endl;
    if ( !t.status() )
      return;
    
    std::cout << "ad_read_handler2" << std::endl;
    if ( !ec )
    {
      std::cout << "ad_read_handler3" << std::endl;
      p.second = bytes_transferred;
      t.get_aspect().template get< ::iow::io::reader::_complete_>()(t, std::move(p));
    }
    else
    {
      ///!! t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
    }
  }
};

struct ad_write_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, ::iow::system::error_code ec , std::size_t bytes_transferred)
  {
    if ( !ec )
    {
      p.second = bytes_transferred;
      t.get_aspect().template get< ::iow::io::writer::_complete_>()(t, std::move(p));
    }
    else
    {
      ///!! t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
    }
  }
};

// Это descriptor::text, у descriptor::jsonrpc хандлеры другие будут
// А также хадлеры разные для dgram и stream
struct ad_incoming_handler
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    std::cout << "ad_incoming_handler1" << std::endl;
    const auto& cntx = t.get_aspect().template get<_context_>();
    if ( cntx.incoming_handler != nullptr )
    {
      std::cout << "ad_incoming_handler2" << std::endl;
      // Проверка на shutdown для tcp::stream и local::stream
      /*
      // Устанавливаеться при инициализации
      // Учесть, что будет сброс при reset (clear)
      
      weak_ptr<T> wthis = t.shared_from_this();
      */
      
      // outgoing_handler без проверок на this и мютекстов outgoing_handler
      // можно захватывать this
      auto callback = cntx.outgoing_handler;
      std::weak_ptr<T> wthis = t.shared_from_this();
      cntx.incoming_handler(std::move(d), t.get_id_(t), t.wrap([wthis, callback](D d){
        if ( auto pthis = wthis.lock() )
        {
          std::lock_guard<typename T::mutex_type> lk( pthis->mutex() );
          if ( callback != nullptr )
          {
            callback(std::move(d));
          }
          else
          {
            pthis->get_aspect().template get<_incoming_>()( *pthis, std::move(d) );
          }
        }
      }));
    }
    else
    {
      t.get_aspect().template get<_incoming_>()( t, std::move(d) );
    }
  }
};
  

struct ad_outgoing_handler
{
  template<typename T>
  void operator()(T& /*t*/, typename T::data_ptr d)
  {
    std::cout << "READ HANDLER " << std::string(d->begin(), d->end() ) << std::endl;
    /*
    if ( t.incoming_handler_ != nullptr )
    {
      outgoung_handler_t callback = t.outgoung_handler_ ;
      
      if ( callback == nullptr )
      {
        weak_ptr<T> wthis = t.shared_from_this();
        callback = t.wrap_([wthis](typename T::data_ptr d) 
        {
          if (auto pthis = wthis.lock() )
          {
            std::lock_guard<typename T::mutex_type> lk(pthis->mutex);
            if ( pthis->status() )
            {
              pthis->get_aspect().template get<_incoming_>()( std::move(d) );
            }
          }
        });
      }
      t.incoming_handler_(t.get_id(), std::move(d), t.outgoung_handler_ )
    }
    else
    {
      t.get_aspect().template get<_incoming_>()( std::move(d) );
    }
    */
  }
};

struct ad_async_read_some
{
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    std::cout << "ad_async_read_some1 " << t.descriptor().native_handle() << std::endl;
    if ( !t.status() )
      return;
    std::cout << "ad_async_read_some2" << std::endl;
    std::weak_ptr<T> wthis = t.shared_from_this();
    auto handler = t.wrap([wthis, p]( iow::system::error_code ec , std::size_t bytes_transferred )
    { 
      std::cout << "ad_async_read_some callback " << bytes_transferred << ec.message() << std::endl;
      if ( auto pthis = wthis.lock() )
      {
        std::cout << "ad_async_read_some native= " << pthis->descriptor().native_handle() << std::endl;
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_read_handler_>()(*pthis, std::move(p), std::move(ec), bytes_transferred);
      }
    });
    
    t.descriptor().async_read_some( ::iow::asio::buffer( p.first, p.second ), std::move(handler) );
  }
};

struct ad_async_write_some
{
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    std::cout << "ad_async_write_some " << t.descriptor().native_handle() << std::endl;
    std::cout << "ad_async_write_some " << p.second << std::endl;
    std::cout << "ad_async_write_some " << (p.first!=nullptr) << std::endl;
    /*
      if ( !t.status() )
        return;
    */
    
    if ( p.first == nullptr || p.second == 0 )
    {
      // Костыль
      // t.descriptor().get_io_service().stop();
      // TODO: _write_ready_
      return;
    }
    
    std::weak_ptr<T> wthis = t.shared_from_this();
    auto callback = t.wrap([wthis, p]( ::iow::system::error_code ec , std::size_t bytes_transferred )
    { 
      if ( auto pthis = wthis.lock() )
      {
        std::cout << "ad_async_write_some callback " << bytes_transferred << ec.message() << std::endl;
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_write_handler_>()(*pthis, std::move(p), std::move(ec), bytes_transferred);
      }
    });
    t.descriptor().async_write_some( ::boost::asio::buffer( p.first, p.second ), callback);
  }
};

  
struct aspect: fas::aspect<
  fas::value< _context_, context >,
  //fas::alias< ::iow::io::stream::_handler_, _incoming_handler_>,
  //fas::alias< ::iow::io::reader::stream::_incoming_, _incoming_handler_>,
  fas::advice< ::iow::io::reader::stream::_incoming_, ad_incoming_handler>,
  fas::alias< _incoming_, ::iow::io::writer::_output_>,
  fas::advice< ::iow::io::reader::_some_, ad_async_read_some>,
  fas::advice< ::iow::io::writer::_some_, ad_async_write_some>,
  fas::advice< _read_handler_, ad_read_handler>, 
  fas::advice< _write_handler_, ad_write_handler>,
  //fas::advice< _incoming_handler_, ad_incoming_handler >,
  fas::advice< _initialize_, ad_initialize>,
  fas::group< ::iow::io::_initialize_, _initialize_>
>{};
  
}}}}
