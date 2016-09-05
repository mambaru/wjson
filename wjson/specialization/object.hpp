//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/object.hpp>
#include <fas/type_list/normalize.hpp>

namespace wjson{

template<typename T, typename L>
struct object
{
  typedef T target;
  typedef serializerT< object<T, L> > serializer;
  typedef typename fas::normalize<L>::type member_list;
};

}
