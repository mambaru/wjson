//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/error.hpp>
#include <wjson/parser.hpp>
#include <fas/integral/bool_.hpp>

namespace wjson{

template<typename V, bool SerQ, bool ReqQ, int R>
class serializerRQ {
public:
  typedef V target;

  template<typename P>
  P operator()( const target& t, P end) const
  {
    if ( SerQ ) *(end++)='"';
    std::copy( t.begin(), t.end(), end );
    if ( SerQ ) *(end++)='"';
    return end;
  }

  template<typename P>
  P operator()( target& t, P beg, P end, json_error* e) const
  {
    if (beg == end)
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( ReqQ && *beg == '"')
      ++beg;
    else if ( ReqQ )
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );
    
    t.clear();
    t.reserve( typename target::size_type(R>=0?R:64));
    P start = beg;
    beg = parser::parse_value(beg, end, e);
    if ( e && *e )
      return beg;
    std::copy( start, beg, std::back_inserter(t) );

    if ( ReqQ && beg == end)
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( ReqQ && *beg == '"')
      ++beg;
    else if ( ReqQ )
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );

    return beg;
  }
};

template<typename J, typename SJ, bool SerQ, bool ReqQ, int R>
class serializerQ {
public:
  typedef serializerQ self;
  typedef typename J::serializer serializer_t;
  typedef typename J::target target;

  template<typename P>
  P operator()( const target& t, P end) const
  {
    return self::serialize(t, end, fas::bool_<SerQ>() );
  }

  template<typename P>
  P operator()( target& t, P beg, P end, json_error* e) const
  {
    return self::deserialize(t, beg, end, e, fas::bool_<ReqQ>() );
  }

private:

  template<typename P>
  static P serialize( const target& t, P end, fas::true_)
  {
    typename SJ::target buf;
    buf.reserve( std::string::size_type(R>=0?R:64) );
    serializer_t()( t, std::back_inserter(buf) );
    return typename SJ::serializer()( buf, end );
  }

  template<typename P>
  static P serialize( const target& t, P end, fas::false_)
  {
    return serializer_t()( t, end );
  }

  template<typename P>
  static P deserialize( target& t, P beg, P end, json_error* e, fas::true_)
  {
    typename SJ::target buf;
    buf.reserve( std::string::size_type(R>=0?R:64) );
    beg = typename SJ::serializer()( buf, beg, end, e );
    if ( e && *e ) return end;

    json_error e2;
    serializer_t()( t, buf.begin(), buf.end(), &e2 );
    if (e && e2)
    {
      *e = json_error( e2.type(), std::distance(beg, end) + e2.tail_of() + 1 );
      return end;
    }
    return beg;
  }

  template<typename P>
  static P deserialize( target& t, P beg, P end, json_error* e, fas::false_)
  {
    if ( parser::is_string(beg, end) )
      return self::deserialize(t, beg, end, e, fas::true_() );
    return serializer_t()( t, beg, end, e );
  }
};

}
