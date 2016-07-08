//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/serializer/flags.hpp>
#include <fas/type_list/normalize.hpp>

namespace wjson{

template< typename T, typename L, char Sep>
struct flags
{
  typedef T target;
  typedef typename fas::normalize<L>::type enum_list;
  typedef serializerT< flags<T, enum_list, Sep> > serializer;
  typedef enum_list member_list;
};

}

