//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/logger/global_log.hpp>

namespace iow{
  
template<typename ...Args>
inline void only_for_log( Args&& ... ){}

}

#ifndef IOW_DISABLE_ALL_LOG

# define IOW_WRITE_LOG( name, type, out ) ::iow::global_log(name, type) << out << std::endl;

#else

# define IOW_WRITE_LOG(name, type, out) ;

#endif

#define IOW_LOG_ERROR( X )         IOW_WRITE_LOG("iow", "error",   X )
#define IOW_LOG_WARNING( X )       IOW_WRITE_LOG("iow", "warning", X )
#define IOW_LOG_MESSAGE( X )       IOW_WRITE_LOG("iow", "message", X )
#define IOW_LOG_FATAL( X )         IOW_WRITE_LOG("iow", "fatal",   X )

