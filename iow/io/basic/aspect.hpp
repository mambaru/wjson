#pragma once

#include <iow/io/basic/ad_start.hpp>
#include <iow/io/basic/ad_stop.hpp>
#include <iow/io/basic/ad_reset.hpp>
#include <iow/io/basic/ad_wrap.hpp>
#include <iow/io/basic/ad_shutdown.hpp>

#include <iow/owner/owner.hpp>
#include <fas/aop.hpp>
#include <mutex>


namespace iow{ namespace io{ namespace basic{

template<typename MutexType = std::recursive_mutex>  
struct aspect: fas::aspect<

  fas::advice< _reset_, ad_reset>,
  fas::advice< _start_, ad_start>,
  fas::advice< _stop_,  ad_stop>,
  fas::stub< _shutdown_>,
  fas::advice< _wrap_,  ad_wrap>,

  fas::value< _io_id_, size_t>,
  fas::value< _status_, bool>,
  fas::value< _owner_, owner>,

  fas::type< _mutex_type_, MutexType>
>{};

}}}
