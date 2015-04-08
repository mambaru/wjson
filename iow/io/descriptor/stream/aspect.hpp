#pragma once

#include <iow/io/stream/stream_options.hpp>
#include <iow/io/aux/data_pool.hpp>
#include <fas/aop.hpp>

extern sembuf op;
namespace iow{ namespace io{ namespace descriptor{ namespace stream{

struct context
{
  typedef std::vector<char> data_type;
  typedef std::unique_ptr<data_type>  data_ptr;
  typedef ::iow::io::data_pool pool_type;
  typedef std::shared_ptr<pool_type> pool_ptr;
  
  typedef std::function< void(data_ptr) > outgoing_handler_fun;
  typedef std::function< void(io_id_t, data_ptr, outgoing_handler_t) > incoming_handler_fun;
  typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_fun;
  typedef std::function< void(io_id_t) > shutdown_handler_fun;

  outgoing_handler_fun outgoing_handler;
  incoming_handler_fun incoming_handler;
  startup_handler_fun  startup_handler;
  shutdown_handler_fun shutdown_handler;
  
};

struct options: 
  public ::iow::io::stream::stream_options<context::data_type>
{
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
    t.get_aspect().template get< ::iow::io::stream::_initialize_>()( t, cref(opt) );
    auto& cntx = t.get_aspect().template get<_context_>();
    cntx.outgoing_handler  = opt.outgoing_handler;
    cntx.incoming_handler  = opt.incoming_handler;
    cntx.startup_handler   = opt.startup_handler;
    cntx.shutdown_handler  = opt.shutdown_handler;
    // TODO: Все осталдьное
  }
};


struct ad_read_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, ::iow::system::error_code ec , std::size_t bytes_transferred)
  {
    if ( !t.status() )
      return;
            
    if ( !ec )
    {
      /*
      d->resize(bytes_transferred);
      t.get_aspect().template get<_ready_>()( t, std::move(d) );
      */
      t.get_aspect().template get< ::iow::io::flow::_complete_>()(t, std::move(dd));
    }
    else
    {
      t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
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
    const auto& cntx = t.get_aspect().template get<_context_>();
    if ( cntx.incoming_handler != nullptr )
    {
      // Проверка на shutdown для tcp::stream и local::stream
      /*
      // Устанавливаеться при инициализации
      // Учесть, что будет сброс при reset (clear)
      
      weak_ptr<T> wthis = t.shared_from_this();
      */
      
      // outgoing_handler без проверок на this и мютекстов outgoing_handler
      // можно захватывать this
      auto callback = cntx.outgoing_handler;
      weak_ptr<T> wthis = t.shared_from_this();
      cntx.incoming_handler(std::move(d), t.get_id_(), t.wrap([wthis, callback](D d){
        if ( auto pthis = wthis.lock() )
        {
          std::lock_guard<typename T::mutex_type> lk( pthis->mutex() );
          if ( callback != nullptr )
          {
            callback(std::move(d));
          }
          else
          {
            pthis->get_aspect().template get<_incoming_>()( std::move(d) );
          }
        }
      }));
    }
    else
    {
      t.get_aspect().template get<_incoming_>()( std::move(d) );
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
    if ( !t.status() )
      return;
    
    std::weak_ptr<T> wthis = t.shared_from_this();
    auto handler = t.wrap([wthis, p]( iow::system::error_code ec , std::size_t bytes_transferred )
    { 
      if ( auto pthis = pthis.lock() )
      {
        typename T::lock_guard lk(pthis->mutex());
        pthis->get_aspect().template get<_read_handler_>()(*pthis, std::move(p), std::move(ec), bytes_transferred);
      }
    });
    
    t.descriptor().async_read_some( ::iow::asio::buffer( p.first, p.second ), std::move(handler) );
  }
};

  
struct aspect: fas::aspect<
  fas::value< _context_, context >,
  fas::alias< ::iow::io::stream::_handler_, _incoming_handler_>
>{};
  
}}}}
