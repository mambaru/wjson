#pragma once

#include <boost/version.hpp>
# if __cplusplus >= 201103L
#   define NO_SCOPED_ENUMS
# endif
# ifdef NO_SCOPED_ENUMS
#   if BOOST_VERSION < 105000
#     ifndef BOOST_NO_SCOPED_ENUMS
#       define BOOST_NO_SCOPED_ENUMS
#       define REMOVE
#     endif
#   else
#     ifndef BOOST_NO_CXX11_SCOPED_ENUMS
#       define BOOST_NO_CXX11_SCOPED_ENUMS
#       define REMOVE
#     endif
#   endif
# endif
# include "boost/filesystem.hpp"
# if defined(NO_SCOPED_ENUMS) && defined(REMOVE)
#   undef REMOVE
#   if BOOST_VERSION < 105000
#     undef BOOST_NO_SCOPED_ENUMS
#   else
#     undef BOOST_NO_CXX11_SCOPED_ENUMS
#   endif
# endif
  
# if __cplusplus >= 201103L
#   define NO_SCOPED_ENUMS
# endif
  
// BOOST_COMPILED_WITH_CXX11 doesn't exist
# if (__cplusplus >= 201103L) && !defined(BOOST_COMPILED_WITH_CXX11)
#   define NO_SCOPED_ENUMS
# endif

#include <boost/system/error_code.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/time_traits.hpp>

namespace iow{ namespace boost{

// Прочие сущности буста
using namespace boost;

}}
