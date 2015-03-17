#pragma once

#include <iow/io/basic/tags.hpp>
#include <iow/owner/owner.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>
#include <mutex>


namespace iow{ namespace io{ namespace basic{

struct ad_start
{
  template<typename T, typename O>
  void operator()(T& t, O options)
  {
    t.get_aspect().template gete<_initialize_>()(t, std::ref(options) );
    t.get_aspect().template get<_status_>()=true;
    t.get_aspect().template gete<_after_start_>()(t);
  }
};

struct ad_stop
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template gete<_before_stop_>()(t);
    t.get_aspect().template get<_reset_>()(t);
    t.get_aspect().template get<_status_>()=false;
    t.get_aspect().template gete<_after_stop_>()(t);
  }
};

struct ad_reset
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "reset" << std::endl;
    t.get_aspect().template get<_owner_>().reset();
    t.get_aspect().template gete<_after_reset_>()(t);
  }
};

struct ad_owner_wrap
{
  template<typename T, typename Handler>
  auto operator()(T& t, Handler h)
    -> decltype( t.get_aspect().template get<_owner_>().wrap( std::move(h) ) )
  {
    return t.get_aspect().template get<_owner_>().wrap( std::move(h) );
  }
};

struct ad_post_wrap
{
  template<typename T, typename Handler>
  void operator()(T& t, Handler h)
  {
  }
  
private:
  template<typename T, typename Handler>
  auto owner_wrap(T& t, Handler h)
    -> typename std::result_of< typename T::aspect::template advice_cast<_owner_wrap_>::type(T&, Handler) >::type
  {
    return t.get_aspect().template get<_owner_wrap_>()(t, h);
  }

  
  /*
  template<typename T, typename Handler>
  void wrap1(T& t, Handler handler)
  {
    return owner_wrap(t, [](){}
  }*/
  
};

struct ad_post
{
  template<typename T, typename Handler>
  void operator()(T& t, Handler)
  {
  }
};

  
template<
  typename MutexType = std::recursive_mutex,
  typename IDType = size_t, 
  typename IOServiceType = ::iow::asio::io_service
>  
struct aspect: fas::aspect<

  fas::advice< _reset_, ad_reset>,
  fas::advice< _start_, ad_start>,
  fas::advice< _stop_,  ad_stop>,
  fas::advice<_owner_wrap_, ad_owner_wrap>,
  fas::advice<_post_wrap_, ad_post_wrap>,
  fas::advice<_post_, ad_post>,
  
  fas::value< _io_id_, IDType>,
  fas::value< _status_, bool>,
  fas::value< _owner_, owner>,
  
  fas::type< _io_service_type_, IOServiceType>,
  fas::type< _mutex_type_, MutexType>
>{};
  
}}}
