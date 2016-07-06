//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/error.hpp>
#include <iow/json/parser.hpp>

#include <sstream>

namespace iow{ namespace json{

template<typename T, int R>
class serializerF
{
public:
  template<typename P>
  P operator()( T v, P end)
  {
    std::stringstream ss;
    const size_t bufsize = ( R == -1 ? 20 : 20 + R ) ;
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

    for (int i = 0; i < bufsize && buf[i]!='\0'; ++i)
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

    std::stringstream ss;
    ss.rdbuf()->pubsetbuf( &(*beg), std::distance(beg, end) );
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

}}
