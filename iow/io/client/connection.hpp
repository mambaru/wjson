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
    auto h = opt.connect_handler;
    std::weak_ptr<T> wthis = t.shared_from_this();
    t.descriptor().async_connect( endpoint, t.wrap([wthis, h](const ::iow::system::error_code& /*ec*/)
    {
      if ( auto p = wthis.lock() )
      {
        
      }
    }));
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
