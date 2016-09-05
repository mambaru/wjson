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

template<typename T, typename L, char Sep>
class serializerT< flags<T, L, Sep> >
{
  typedef typename flags<T, L>::enum_list enum_list;
public:
  template<typename P>
  P operator()( const T& v, P end)
  {
    *(end++)= ( Sep == ',' ? '[' : '"' );
    end = this->serialize(v, enum_list(), end,true);
    *(end++)=( Sep == ',' ? ']' : '"' );
    return end;
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *beg == '[' )
    {
      if ( Sep!=',' )
        return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );
    }
    else if ( *beg != '"' )
      return create_error<error_code::ExpectedOf>(e, end, "[", std::distance(beg, end) );

    beg = parser::parse_space(++beg, end, e);
    if ( beg==end )
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    beg = this->deserialize(v, beg, end, e);

    if ( beg==end )
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *beg != ']' && *beg!='"'  )
      return create_error<error_code::ExpectedOf>(e, end, "]", std::distance(beg, end) );
    ++beg;

    return beg;
  }

private:

  template<typename LL, typename RR, typename P>
  P serialize( const T& v, fas::type_list<LL, RR>, P end, bool isFirst)
  {
    if (LL::value & v)
    {
      if (!isFirst) 
        *(end++) = Sep;

      if ( Sep==',' )
        *(end++)='"';

      const char* val = LL()();

      for ( ; *val!='\0' ; ++val) 
        *(end++) = *val;

      if ( Sep==',' )
        *(end++)='"';

      return this->serialize(v, RR(), end, false);
    }
    else
    {
      return this->serialize(v, RR(), end, isFirst);
    }
  }

  template<typename P>
  P serialize( const T&, fas::empty_list, P end, bool)
  {
    return end;
  }

  template<typename LL, typename RR, typename P>
  P deserialize_one( T& v, P beg, P end, json_error* e, fas::type_list<LL, RR>)
  {
    if ( beg==end ) return beg;
    P cur = beg;

    if ( Sep==',' ) ++cur;
    const char *pstr = LL()();
    for ( ; cur!=end && *pstr!='\0' && *pstr==*cur; ++cur, ++pstr);

    if ( *pstr == '\0' && cur!=end )
    {
      if ( parser::is_space(cur, end) || *cur==Sep || *cur=='"' || *cur==']' )
      {
        v = static_cast<T>(v|LL::value);
        beg = cur;
        if ( Sep==',' ) ++beg;
        beg = parser::parse_space(beg, end, e);
        if ( beg == end )
          return beg;
        if ( *beg != Sep )
          return beg;
        ++beg;
        beg = parser::parse_space(beg, end, e);

      }
    }
    return this->deserialize_one(v, beg, end, e, RR() );
  }

  template<typename P>
  P deserialize_one( T&, P beg, P, json_error*, fas::empty_list)
  {
    return beg;
  }

  template<typename P>
  P deserialize( T& v, P beg, P end, json_error* e) 
  {
    const char fin = ( Sep == ',' ) ? ']' : '"';
    while ( beg!=end && *beg!=fin )
    {
      if ( beg == end )
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if ( *beg == fin )
        return beg;

      P cur = beg;
      beg = this->deserialize_one(v, beg, end, e, enum_list() );
      if ( cur == beg )
        return create_error<error_code::InvalidEnum>(e, end, std::distance(beg, end));
      beg = parser::parse_space(beg, end, e);
      if ( beg == end )
        return create_error<error_code::UnexpectedEndFragment>(e, end);
    }
    return beg;
  }
};

}

