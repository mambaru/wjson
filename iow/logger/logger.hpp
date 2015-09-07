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
#   define IOW_WRITE_PROGRESS( name, type, out ) ::iow::global_log(name, type) << out << "\r";
#else
#   define IOW_WRITE_LOG(name, type, out)
#   define IOW_WRITE_PROGRESS( name, type, out )
#endif

#define IOW_WRITE_LOG_ERROR( name, X )         IOW_WRITE_LOG(name, "ERROR",   X )
#define IOW_WRITE_LOG_WARNING( name, X )       IOW_WRITE_LOG(name, "WARNING", X )
#define IOW_WRITE_LOG_MESSAGE( name, X )       IOW_WRITE_LOG(name, "MESSAGE", X )
#define IOW_WRITE_LOG_FATAL( name, X )         IOW_WRITE_LOG(name, "FATAL",   X )
#define IOW_WRITE_LOG_BEGIN( name, X )         IOW_WRITE_LOG(name, "BEGIN",   X )
#define IOW_WRITE_LOG_END( name, X )           IOW_WRITE_LOG(name, "END",   X )
#define IOW_WRITE_LOG_PROGRESS( name, X )      IOW_WRITE_PROGRESS(name, "PROGRESS",   X )

#ifdef NDEBUG 
# ifdef IOW_ENABLE_DEBUG_LOG          // включить в Release режиме
#   define IOW_WRITE_LOG_DEBUG( name, X )      IOW_WRITE_LOG(name, "DEBUG",   X )
#   define IOW_WRITE_LOG_TRACE( name, X )      IOW_WRITE_LOG(name, "TRACE",   X )
# else
#   define IOW_WRITE_LOG_DEBUG( name, X )
#   define IOW_WRITE_LOG_TRACE( name, X )         
# endif
#else
# ifdef IOW_DISABLE_DEBUG_LOG         // отключить в Debug режиме
#   define IOW_WRITE_LOG_DEBUG( name, X )
#   define IOW_WRITE_LOG_TRACE( name, X )         
# else
#   define IOW_WRITE_LOG_DEBUG( name, X )      IOW_WRITE_LOG(name, "DEBUG",   X )
#   define IOW_WRITE_LOG_TRACE( name, X )      IOW_WRITE_LOG(name, "TRACE",   X )
# endif
#endif

#define JSONRPC_LOG_ERROR(X)    IOW_WRITE_LOG_ERROR( "jsonrpc", X )
#define JSONRPC_LOG_WARNING(X)  IOW_WRITE_LOG_WARNING( "jsonrpc", X )
#define JSONRPC_LOG_MESSAGE(X)  IOW_WRITE_LOG_MESSAGE( "jsonrpc", X )
#define JSONRPC_LOG_FATAL(X)    IOW_WRITE_LOG_FATAL( "jsonrpc", X )
#define JSONRPC_LOG_BEGIN(X)    IOW_WRITE_LOG_BEGIN( "jsonrpc", X )
#define JSONRPC_LOG_END(X)      IOW_WRITE_LOG_END( "jsonrpc", X )
#define JSONRPC_LOG_DEBUG(X)    IOW_WRITE_LOG_DEBUG( "jsonrpc", X )
#define JSONRPC_LOG_TRACE(X)    IOW_WRITE_LOG_TRACE( "jsonrpc", X )
#define JSONRPC_LOG_PROGRESS(X) IOW_WRITE_LOG_PROGRESS( "jsonrpc", X )


#define IOW_LOG_ERROR(X)    IOW_WRITE_LOG_ERROR( "iow", X )
#define IOW_LOG_WARNING(X)  IOW_WRITE_LOG_WARNING( "iow", X )
#define IOW_LOG_MESSAGE(X)  IOW_WRITE_LOG_MESSAGE( "iow", X )
#define IOW_LOG_FATAL(X)    IOW_WRITE_LOG_FATAL( "iow", X )
#define IOW_LOG_BEGIN(X)    IOW_WRITE_LOG_BEGIN( "iow", X )
#define IOW_LOG_END(X)      IOW_WRITE_LOG_END( "iow", X )
#define IOW_LOG_DEBUG(X)    IOW_WRITE_LOG_DEBUG( "iow", X )
#define IOW_LOG_TRACE(X)    IOW_WRITE_LOG_TRACE( "iow", X )
#define IOW_LOG_PROGRESS(X) IOW_WRITE_LOG_PROGRESS( "iow", X )
