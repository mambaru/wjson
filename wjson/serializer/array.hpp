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

template<typename C, typename R>
struct array_r;

template<char C>
struct ch2str;

template<>
struct ch2str<'{'>{ const char* operator()() const { return "{"; } };

template<>
struct ch2str<'}'>{ const char* operator()() const { return "}"; } };

template<>
struct ch2str<'['>{ const char* operator()() const { return "["; } };

template<>
struct ch2str<']'>{ const char* operator()() const { return "]"; } };

template< typename T, char L, char R >
class serializerA
{
  typedef T array_type;
  typedef typename array_type::target_container target_container;
  typedef typename array_type::json_value json_value;
  typedef typename json_value::serializer serializer;
  typedef typename json_value::target target;

public:
  
  template<typename P>
  P operator()( target_container& t,  P beg, P end, json_error* e) const
  {
    t.clear();
    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end, e);

    array_type().reserve(t);

    typename array_type::inserter_iterator bitr = array_type::inserter(t);

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if (*(beg++)!=L) 
      return create_error<error_code::ExpectedOf>(e, end, ch2str<L>()(), std::distance(beg, end)+1 );

    for (;beg!=end;)
    {
      beg = parser::parse_space(beg, end, e);
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if (*beg==R) break;
      target tg;
      beg = serializer()( tg, beg, end, e);
#if __cplusplus >= 201103L
      *(bitr++) = std::move(tg);
#else
      *(bitr++) = tg;
#endif
      beg = parser::parse_space(beg, end, e);
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if (*beg==R) break;
      if (*beg!=',')
        return create_error<error_code::ExpectedOf>(e, end, ",", std::distance(beg, end) );
      ++beg;
    }
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    if (*beg!=R) 
      return create_error<error_code::ExpectedOf>(e, end, ch2str<R>()(), std::distance(beg, end) );
    ++beg;
    return beg;
  }

  template<typename P>
  P operator()( const target_container& t, P end) const
  {
    *(end++)=L;
    typename target_container::const_iterator itr = t.begin();
    for (;itr!=t.end();)
    {
      end = serializer()(*itr, end);
      ++itr;
      if (itr!=t.end()) *(end++)=',';
    }
    *(end++)=R;
    return end;
  }
};

template< typename T, typename RR >
class serializerT< array_r<T, RR> >
  : public serializerA< array_r<T, RR>, '[', ']'>
{
};

#if __cplusplus >= 201103L

template< typename J, size_t N, typename RR, char L, char R >
class serializerA< array_r< std::array<J,N>, RR>, L, R >
{
  typedef array_r<std::array<J,N>, RR> array_type;
  typedef typename array_type::target_container target_container;
  typedef typename array_type::json_value json_value;
  typedef typename json_value::serializer serializer;
  typedef typename json_value::target target;

public:

  template<typename P>
  P operator()( target_container& t,  P beg, P end, json_error* e) const
  {
    if ( parser::is_null(beg, end) )
    {
      for (size_t i = 0; i < N ; ++i)
      {
        // null для каждого элемента
        serializer()(t[i], beg, end, e);
      }
      return parser::parse_null(beg, end, e);
    }

    target* bitr = &t[0];
    target* eitr = bitr + N;

    if (beg==end)
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if (*(beg++)!=L) 
      return create_error<error_code::ExpectedOf>(e, end, ch2str<L>()(), std::distance(beg, end) + 1 );

    for (;beg!=end && bitr!=eitr;)
    {
      beg = parser::parse_space(beg, end);
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if (*beg==R) 
        break;

      target tg;
      beg = serializer()( tg, beg, end, e);
      *(bitr++) = tg;
      beg = parser::parse_space(beg, end);
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if (*beg==R) 
        break;
      if (*beg!=',') 
        return create_error<error_code::ExpectedOf>(e, end, "", std::distance(beg, end) );
      ++beg;
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if (*(beg++)!=R) 
      return create_error<error_code::ExpectedOf>(e, end, ch2str<R>()(), std::distance(beg, end) + 1 );
    return beg;
  }

  template<typename P>
  P operator()( const target_container& t, P end) const
  {
    *(end++)=L;
    const target* itr = &t[0];
    const target* iend = itr + N;
    for (;itr!=iend;)
    {
      end = serializer()(*itr, end);
      ++itr;
      if (itr!=iend) *(end++)=',';
    }
    *(end++)=R;
    return end;
  }
};
#endif // __cplusplus >= 201103L

template< typename J, int N, typename RR, char L, char R >
class serializerA< array_r< J[N], RR>, L, R >
{
  typedef array_r< J[N], RR> array_type;
  typedef typename array_type::target_container target_container;
  typedef typename array_type::json_value json_value;
  typedef typename json_value::serializer serializer;
  typedef typename json_value::target target;

public:

  template<typename P>
  P operator()( target_container& t,  P beg, P end, json_error* e) const
  {
    if ( parser::is_null(beg, end) )
    {
      for (int i = 0; i < N ; ++i)
      {
        // null для каждого элемента
        serializer()(t[i], beg, end, e);
      }
      return parser::parse_null(beg, end, e);
    }

    target* bitr = t;
    target* eitr = bitr + N;

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    if (*beg!=L) 
      return create_error<error_code::ExpectedOf>(e, end, ch2str<L>()(), std::distance(beg, end) );

    ++beg;
    for (;beg!=end && bitr!=eitr;)
    {
      beg = parser::parse_space(beg, end, e);
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if (*beg==R) break;

      beg = serializer()( *(bitr++), beg, end, e);
      beg = parser::parse_space(beg, end, e);
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if (*beg==R) break;
      if (*beg!=',') 
        return create_error<error_code::ExpectedOf>(e, end, ",", std::distance(beg, end) );
      ++beg;
    }
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if (*(beg++)!=R) 
      return create_error<error_code::ExpectedOf>(e, end, ch2str<R>()(), std::distance(beg, end) + 1 );

    return beg;
  }

  template<typename P>
  P operator()( const target_container& t, P end) const
  {
    *(end++)=L;
    const target* itr = t;
    const target* iend = itr + N;
    for (;itr!=iend;)
    {
      end = serializer()(*itr, end);
      ++itr;
      if (itr!=iend) *(end++)=',';
    }
    *(end++)=R;
    return end;
  }
};

}
