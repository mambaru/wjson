//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/error.hpp>
#include <wjson/parser.hpp>

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
    if ( write_if_(beg, v%fractions, 'm') )
      *(beg++) = 's';
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
      if (!e) return beg;
      switch( *beg ) // строка валидная, так что за пределы не вышли,
      {
        case 'd': v += cur * 86400 * fractions; break;
        case 'h': v += cur * 3600 * fractions; break;
        case 'm':
          ++beg;
          if (beg==end)
            return create_error<error_code::UnexpectedEndFragment>( e, end );
          if ( *beg!='s')
          {
            v += cur * 60 * fractions;
            continue;
          } // миллисекунды
          v += cur;
          break;
        case 's':
          v += cur * fractions;
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
  value_type write_if_(P& beg, const value_type& value, char ch)
  {
    if ( value != 0)
    {
      typename value<value_type>::serializer()(value, beg);
      *(beg++)=ch;
    }
    return value;
  }
};

}
