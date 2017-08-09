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

template<typename T, typename L>
class serializerT< enumerator<T, L> >
{
  typedef typename enumerator<T, L>::enum_list enum_list;
  typedef serializerT< enumerator<T, L> > self;
public:

  template<typename P>
  P operator()( const T& v, P end)
  {
    *(end++)='"';
    end = this->serialize(v, enum_list(), end);
    *(end++)='"';
    return end;
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    v = T();
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    if (*beg!='"') 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );

    ++beg;
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    P first = beg;
    for ( ; beg!=end && *beg!='"'; ++beg);
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    if (*beg!='"') 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );

    if ( !this->deserialize(v, enum_list(), first, beg) )
      return create_error<error_code::InvalidEnum>(e, end, std::distance(first, end) );
    ++beg;
    return beg;
  }

private:

  template<typename LL, typename RR, typename P>
  static P serialize( const T& v, fas::type_list<LL, RR>, P end)
  {
    if (LL::value == v)
    {
      const char* val = LL()();
      for ( ; *val!='\0' ; ++val)
        *(end++) = *val;
      return end;
    }
    else
    {
      return self::serialize(v, RR(), end);
    }
  }

  template<typename P>
  static P serialize( const T& , fas::empty_list, P end)
  {
    return end;
  }


  template<typename LL, typename RR, typename P>
  static bool deserialize( T& v, fas::type_list<LL, RR>, P beg, P end)
  {
    P first = beg;
    const char *pstr = LL()();
    for ( ; beg!=end && *pstr!='\0' && *pstr==*beg; ++beg, ++pstr);
    if ( beg==end && *pstr=='\0')
    {
      v = LL::value;
      return true;
    }
    return self::deserialize(v, RR(), first, end);
  }

  template<typename P>
  static bool deserialize( T& , fas::empty_list, P , P )
  {
    return false;
  }
};

}

