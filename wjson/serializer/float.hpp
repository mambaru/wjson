//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/error.hpp>
#include <wjson/parser.hpp>

#include <sstream>
#include <cstring>

namespace wjson{

template<typename T, int R>
class serializerF
{
public:
  static const std::ptrdiff_t bufsize = ( R == -1 ? 20 : 20 + R ) ;

  template<typename P>
  P operator()( T v, P end)
  {
    std::stringstream ss;
    char buf[bufsize]={'\0'};
    ss.rdbuf()->pubsetbuf(buf, bufsize);
    if ( R == -1 ) 
    {
      ss << std::scientific;
    }
    else
    {
      ss << std::fixed;
      ss.precision(R);
    }

    ss << v ;

    for (size_t i = 0; i < bufsize && buf[i]!='\0'; ++i)
    {
      *(end++) = buf[i];
    }
    return end;
  }
  
  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    if( beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    if ( parser::is_null(beg, end) )
    {
      v = T();
      return parser::parse_null(beg, end, e);
    }

    if ( !parser::is_number(beg, end) )
    {
      return create_error<error_code::InvalidNumber>( e, end, std::distance(beg, end) );
    }

    std::ptrdiff_t dist = std::distance(beg, end);
    char buf[bufsize+1]={'\0'};
    if ( dist > bufsize )  dist = bufsize;
    std::memcpy(buf, &(*beg), dist);
    std::stringstream ss( buf );
    //ss.rdbuf()->pubsetbuf( &(*beg), std::distance(beg, end) );
    ss >> v;
    return parser::parse_number(beg, end, e);
  }
};


template<int R>
class serializerT< value<float, R> >
  : public serializerF<float, R>
{
};

template<int R>
class serializerT< value<double, R> >
  : public serializerF<double, R>
{
};

template<int R>
class serializerT< value<long double, R> >
  : public serializerF<long double, R>
{
};

}
