//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//


#pragma once

#include <wjson/serializer/enumerator.hpp>
#include <fas/type_list/normalize.hpp>

namespace wjson{

template< typename T, typename L>
struct enumerator
{
  typedef T target;
  typedef typename fas::normalize<L>::type enum_list;
  typedef serializerT< enumerator<T, enum_list> > serializer;
  typedef enum_list member_list;
};

}

