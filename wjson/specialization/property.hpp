//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/object.hpp>
#include <wjson/serializer/member_value.hpp>
#include <fas/type_list/normalize.hpp>

namespace wjson{

template<typename T, typename M, M T::* m>
struct property
{
  void operator()(T& t, const M& value ) const
  {
    t.*m = value;
  }

  const M& operator()(const T& t) const
  {
    return t.*m;
  }
};

}
