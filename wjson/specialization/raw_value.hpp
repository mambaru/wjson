//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/raw_value.hpp>

namespace wjson{

template<typename T, int R>
struct raw_value
{
  typedef T target;
  typedef serializerT< raw_value< T, R > > serializer;
};

}

