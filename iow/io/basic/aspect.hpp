#pragma once

#include <iow/io/basic/ad_start.hpp>
#include <iow/io/basic/ad_stop.hpp>
#include <iow/io/basic/ad_reset.hpp>
#include <iow/io/basic/ad_wrap.hpp>
#include <iow/io/basic/ad_shutdown.hpp>
#include <iow/io/basic/ad_create_id.hpp>
#include <iow/io/basic/context.hpp>

#include <iow/owner/owner.hpp>
#include <fas/aop.hpp>
#include <mutex>

namespace iow{ namespace io{ namespace basic{

template<typename MutexType /*= std::recursive_mutex */>  
struct aspect: fas::aspect<
  fas::advice< _reset_,     ad_reset>,
  fas::advice< _start_,     ad_start>,
  fas::advice< _stop_,      ad_stop>,
  fas::advice< _shutdown_,  ad_shutdown>,
  fas::advice< _wrap_,      ad_wrap>,
  fas::advice< _create_id_, ad_create_id>,
  fas::value< _context_,    context<MutexType> >
>{};

}}}
