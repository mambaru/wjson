#pragma once

#include <iow/io/connection/connection.hpp>


namespace iow{ namespace io{ namespace client{
  
struct _connect_;
struct _sync_resolver_;


struct ad_connect
{
  template<typename T, typename Opt>
  void operator()(T& t, const Opt& opt)
  {
    const auto endpoint = t.get_aspect().template get<_sync_resolver_>()(t, opt);
    auto ch = opt.connect_handler;
    auto er = opt.error_handler;
    std::weak_ptr<T> wthis = t.shared_from_this();
    
    auto tmp = t.wrap([wthis, opt](const ::iow::system::error_code& ec)
    {
      if ( auto p = wthis.lock() )
      {
        if (!ec)
        {
          std::cout << "OK MAN " << opt.addr <<":"<< opt.port << ":" << ec.message() << std::endl;
          if ( opt.connect_handler )
          {
            opt.connect_handler();
          }
        }
        else
        {
          if ( opt.error_handler )
          {
            std::cout << "FUCK MAN " << ec.message() << std::endl;
            opt.error_handler(ec);
          }
        }
      }
    });
#warning async_connect всегда возвращает OK. Поймать можно только при попытке прочитать
    ::iow::system::error_code ec;
    t.descriptor().connect( endpoint, ec);
    tmp(ec);
    //::iow::asio::async_connect(t.descriptor(), endpoint, tmp);
  }
};

typedef fas::aspect< 
  fas::advice<_connect_, ad_connect>
> aspect;

template<typename A>
using connection_base = ::iow::io::connection::connection_base< typename fas::merge_aspect<A, aspect>::type >;

template<typename A = fas::aspect<> >
class connection
  : public connection_base<A>
  , public std::enable_shared_from_this< connection<A> >
{
public: 
   typedef connection_base<A> super;
   typedef typename super::descriptor_type descriptor_type;
   typedef typename super::mutex_type mutex_type;
  
  connection(descriptor_type&& desc)
    : super( std::move( desc ) )
  {}
  
  
  template<typename Opt>
  void connect(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    this->get_aspect().template get<_connect_>()(*this, std::forward<Opt>(opt) );
  }

  
  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::start_(*this, opt.connection);
  }
  

};
  
}}}
