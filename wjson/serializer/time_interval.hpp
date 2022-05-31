//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2019, 2022
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/error.hpp>
#include <wjson/parser.hpp>
#include <fas/system/nullptr.hpp>

namespace wjson{

template<typename T, T fractions>
class serializerT< time_interval<T, fractions> >
{
  typedef T value_type;
public:
  template<typename P>
  P operator()( const T& v, P beg)
  {
    value_type seconds = v/fractions ;
    *(beg++) = '"';
    value_type days = write_if_(beg, seconds/(86400), 'd');
    value_type hours = write_if_(beg, (seconds - days*86400)/3600, 'h');
    write_if_(beg, (seconds - hours*3600 - days*86400)/60, 'm');
    write_if_(beg,  seconds%60, 's');
    if ( v%fractions != 0 )
    {
      value_type tail = v%fractions;
      value_type ms = (tail*1000)/fractions;
      value_type mks = tail - ms*1000;
      mks = (mks*1000000)/fractions;
      if ( ms > 0 && write_if_(beg, ms, 'm') )
        *(beg++) = 's';
      if ( mks > 0 && write_if_(beg, mks, 'm') )
        { *(beg++) = 'k'; *(beg++) = 's'; }
    }
    *(beg++) = '"';

    return beg;
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    if (beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    v = T();

    if ( parser::is_null(beg, end) )
    {
      v = T();
      return parser::parse_null(beg, end, e);
    }

    if ( parser::is_number(beg, end) )
      return typename value<T>::serializer()(v, beg, end, e);
    else if ( !parser::is_string(beg, end))
      return create_error<error_code::InvalidString>( e, end );

    for ( ++beg; beg!=end && *beg!='"'; )
    {
      T cur = T();
      beg = typename value<T>::serializer()(cur, beg, end, e);
      if ( (e!=fas_nullptr && *e) || (beg==end) ) return beg;
      switch( *beg ) 
      {
        case 'd': v += cur * 86400 * fractions; break; // day
        case 'h': v += cur * 3600 * fractions; break;  // hours
        case 'm':
          ++beg;
          if ( beg==end )
          {
              v += cur * 60 * fractions; // m
              continue; 
          }
          switch ( *beg )
          {
            case 's': 
              v += (cur*fractions)/1000; 
              ++beg; 
              continue; // ms
            case 'k': 
              v += (cur*fractions)/1000000; 
              ++beg; 
              ++beg; 
              continue; // mks
            default: v += cur * 60 * fractions; continue; // m
          };
          continue;
        case 's':
          v += cur * fractions; // s
          break;
        default:
          break;
      }
      ++beg;
    }

    if (beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );
    if ( *beg != '"')
      return create_error<error_code::InvalidString>( e, end );
    ++beg;
    return beg;
  }

private:

  template<typename P>
  value_type write_if_(P& beg, const value_type& v, char ch)
  {
    if ( v != 0)
    {
      typename wjson::value<value_type>::serializer()(v, beg);
      *(beg++)=ch;
    }
    return v;
  }
};

}
