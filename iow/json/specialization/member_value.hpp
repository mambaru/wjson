//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/serializer/member_value.hpp>

namespace iow{ namespace json{


template<typename T, typename V, typename M, M V::* m, typename W >
struct member_value
{
  typedef T target;
  typedef serializerT< member_value<T, V, M, m, W> > serializer;
};

}}

