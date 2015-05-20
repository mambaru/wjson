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
#   define IOW_WRITE_LOG( name, type, out ) ::iow::global_log(name, type) << out << std::endl;
#   define IOW_WRITE_LOG_PROGRESS( name, type, out ) ::iow::global_log(name, type) << out << "\r";
#else
#   define IOW_WRITE_LOG(name, type, out)
#   define IOW_WRITE_LOG_PROGRESS( name, type, out )
#endif

#define IOW_LOG_ERROR( name, X )         IOW_WRITE_LOG(name, "ERROR",   X )
#define IOW_LOG_WARNING( name, X )       IOW_WRITE_LOG(name, "WARNING", X )
#define IOW_LOG_MESSAGE( name, X )       IOW_WRITE_LOG(name, "MESSAGE", X )
#define IOW_LOG_FATAL( name, X )         IOW_WRITE_LOG(name, "FATAL",   X )
#define IOW_LOG_BEGIN( name, X )         IOW_WRITE_LOG(name, "BEGIN",   X )
#define IOW_LOG_END( name, X )           IOW_WRITE_LOG(name, "END",   X )
#define IOW_LOG_PROGRESS( name, X )      IOW_WRITE_LOG_PROGRESS(name, "PROGRESS",   X )

#ifdef NDEBUG 
# ifdef IOW_ENABLE_DEBUG_LOG          // включить в Release режиме
#   define IOW_LOG_DEBUG( name, X )      IOW_WRITE_LOG(name, "DEBUG",   X )
#   define IOW_LOG_TRACE( name, X )      IOW_WRITE_LOG(name, "TRACE",   X )
# else
#   define IOW_LOG_DEBUG( name, X )
#   define IOW_LOG_TRACE( name, X )         
# endif
#else
# ifdef IOW_DISABLE_DEBUG_LOG         // отключить в Debug режиме
#   define IOW_LOG_DEBUG( name, X )
#   define IOW_LOG_TRACE( X )         
# else
#   define IOW_LOG_DEBUG( name, X )      IOW_WRITE_LOG(name, "DEBUG",   X )
#   define IOW_LOG_TRACE( X )            IOW_WRITE_LOG(name, "TRACE",   X )
# endif
#endif

