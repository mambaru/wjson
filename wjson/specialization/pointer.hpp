//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/pointer.hpp>

namespace wjson{

template<typename T, typename J>
struct pointer
{
  typedef T target;
  typedef serializerT< pointer< T, J > > serializer;
};

}

