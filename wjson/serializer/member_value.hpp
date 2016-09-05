//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace wjson{

  template<typename T, typename V, typename M, M V::* m, typename W >
struct serializerT< member_value<T, V, M, m, W> >
{
  template<typename P>
  P operator()( const T& t, P end)
  {
    return typename W::serializer()( static_cast<const V&>(t).*m, end);
  }

  template<typename P>
  P operator()( T& t, P beg, P end, json_error* e)
  {
    return typename W::serializer()( static_cast<V&>(t).*m, beg, end, e);
  }
};

}
