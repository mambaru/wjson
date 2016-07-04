//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/error.hpp>
#include <iow/json/parser.hpp>
#include <algorithm>

namespace iow{ namespace json{

template<typename T>
class serializerT< iterator_pair<T> >
{
public:
  template<typename P>
  P operator()( const T& v, P end )
  {
    if ( v.first != v.second )
      return std::copy(v.first, v.second, end );
    else
    {
      *(end++)='"';
      *(end++)='"';
      return end;
    }
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    v.first = beg;
    beg = parser::parse_value(beg, end, e);
    v.second = beg;
    return beg;
  }
};


}}
