//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/error.hpp>
#include <iow/json/parser.hpp>

namespace iow{ namespace json{

template<>
class serializerT< value<bool> >
{

public:
  template<typename P>
  P operator()( bool v, P beg)
  {
    if ( v ) { *(beg++)='t'; *(beg++)='r';*(beg++)='u';*(beg++)='e'; }
    else { *(beg++)='f'; *(beg++)='a';*(beg++)='l';*(beg++)='s'; *(beg++)='e'; }
    return beg;
  }

  template<typename P>
  P operator() ( bool& v, P beg, P end, json_error* e )
  {
    if (beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    if ( parser::is_null(beg, end) )
    {
      v = bool();
      return parser::parse_null(beg, end, e);
    }

    v = *beg == 't';
    if ( v )
      return parser::parse_bool(beg, end, e);

    if ( *beg == 'f' )
      return parser::parse_bool(beg, end, e);

    return create_error<error_code::InvalidBool>( e, end, std::distance(beg, end) );

  }
};

}}
