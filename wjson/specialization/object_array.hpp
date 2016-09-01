//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/object_array.hpp>
#include <fas/type_list/normalize.hpp>

namespace wjson{

template<typename T, typename L>
struct object_array
{
  typedef T target;
  typedef serializerT< object_array<T, L> > serializer;
  typedef typename fas::normalize<L>::type member_list;
};

}
