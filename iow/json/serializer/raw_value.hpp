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

template<typename T, int R>
class serializerT< raw_value<T, R> >
{
public:
  template<typename P>
  P operator()( const T& v, P end)
  {
    if ( v.begin() != v.end() )
    {
      return std::copy(v.begin(), v.end(), end );
    }
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
    v.clear();
    P start = beg;
    beg = parser::parse_value(beg, end, e);
    std::copy( start, beg, std::back_inserter(v) );
    return beg;
  }
};
}}
