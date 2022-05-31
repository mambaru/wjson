//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/time_interval.hpp>
#include <fas/type_list/normalize.hpp>

namespace wjson{

template<typename T, T fractions>
struct time_interval
{
  typedef T target;
  typedef serializerT< time_interval<T, fractions> > serializer;
};

template<typename T>
struct time_interval_s: time_interval<T, 1>{};

template<typename T>
struct time_interval_ms: time_interval<T, 1000>{};

template<typename T>
struct time_interval_mks: time_interval<T, 1000000>{};

}
