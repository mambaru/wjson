//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//


#pragma once

#include <wjson/predef.hpp>
#include <wjson/parser.hpp>
#include <wjson/error.hpp>

namespace wjson{

template<typename K, typename V>
class serializerT< field<K, V> >
{
public:
  typedef field<K, V> pair_type;
  typedef typename pair_type::target target;
  typedef typename pair_type::key_serializer key_serializer;
  typedef typename pair_type::value_serializer value_serializer;

  template<typename P>
  P operator()( const target& t, P end)
  {
    end = key_serializer()(t.first, end );
    *(end++)=':';
    end = value_serializer()(t.second, end );
    return end;
  }

  template<typename P>
  P operator()( target& t,  P beg, P end, json_error* e)
  {
    beg = key_serializer()(t.first, beg, end, e );
    beg = parser::parse_space(beg, end, e);
    if ( beg==end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    if ( *(beg++)!=':' ) 
      return create_error<error_code::ExpectedOf>(e, end, ":", std::distance(beg, end)+1 );
    beg = parser::parse_space(beg, end, e);
    if ( beg==end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    beg = value_serializer()(t.second, beg, end, e );
    return beg;
  }
};

}
