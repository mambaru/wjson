#pragma once

#include <iow/io/basic/ad_start.hpp>
#include <iow/io/basic/ad_stop.hpp>
#include <iow/io/basic/ad_reset.hpp>
#include <iow/io/basic/ad_wrap.hpp>
#include <iow/io/basic/ad_shutdown.hpp>
#include <iow/io/basic/context.hpp>

#include <iow/owner/owner.hpp>
#include <fas/aop.hpp>
#include <mutex>

namespace iow{ namespace io{ namespace basic{

struct ad_create_id
{
  template<typename T>
  void operator()(T& t)
  {
    typedef typename T::aspect::template advice_cast<_context_>::type context_type;
    typedef typename context_type::io_id_type io_id_type;
    
    t.get_aspect().template get<_context_>().io_id = create_id<io_id_type>();
    /*
    t.get_aspect().template gete<_before_start_>()(t);
    t.get_aspect().template get<_io_id_>() = create_id< typename T::io_id_t>();
    t.get_aspect().template gete<_initialize_>()(t, options );
    t.get_aspect().template get<_status_>()=true;
    t.get_aspect().template gete<_after_start_>()(t);
    */
  }
};


template<typename MutexType /*= std::recursive_mutex */>  
struct aspect: fas::aspect<

  fas::advice< _reset_,     ad_reset>,
  fas::advice< _start_,     ad_start>,
  fas::advice< _stop_,      ad_stop>,
  fas::advice< _shutdown_,  ad_shutdown>,
  fas::advice< _wrap_,      ad_wrap>,
  fas::advice< _create_id_, ad_create_id>,
  fas::value< _context_,    context<MutexType> >
  
  /*
  fas::value< _io_id_, size_t>,
  fas::value< _status_, bool>,
  fas::value< _owner_, owner>,

  fas::type< _mutex_type_, MutexType>
  */
>{};

}}}
