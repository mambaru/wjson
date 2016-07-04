//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/serializer/pointer.hpp>

namespace iow{ namespace json{

template<typename T, typename J>
struct pointer
{
  typedef T target;
  typedef serializerT< pointer< T, J > > serializer;
};

}}

