//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/serializer/object_array.hpp>
#include <fas/type_list/normalize.hpp>

namespace iow{ namespace json{

template<typename T, typename L>
struct object_array
{
  typedef T target;
  typedef serializerT< object_array<T, L> > serializer;
  typedef typename fas::normalize<L>::type member_list;
};

}}
