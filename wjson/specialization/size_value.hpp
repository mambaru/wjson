//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/size_value.hpp>
#include <fas/type_list/normalize.hpp>

namespace wjson{

template<typename T, T metric>
struct size_value
{
  typedef T target;
  typedef serializerT< size_value<T, metric> > serializer;
};

}
