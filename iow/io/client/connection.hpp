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
    t.descriptor().connect(endpoint);
  }
};

typedef fas::aspect< 
  fas::advice<_connect_, ad_connect>
> aspect;
  
template<typename A = fas::aspect<> >
class connection
  : public ::iow::io::connection::connection< typename fas::merge_aspect<A, aspect>::type >
{
public: 
   typedef ::iow::io::connection::connection< typename fas::merge_aspect<A, aspect>::type > super;
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
    super::start(opt.connection);
  }
  

};
  
}}}
