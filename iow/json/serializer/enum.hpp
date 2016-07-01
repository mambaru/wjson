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

template<typename T, typename L>
class serializerT< enumerator<T, L> >
{
  typedef typename enumerator<T, L>::enum_list enum_list;
  
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

    this->deserialize(v, enum_list(), first, beg);
    ++beg;
    return beg;
  }

private:

  template<typename LL, typename RR, typename P>
  P serialize( const T& v, fas::type_list<LL, RR>, P end)
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
      return this->serialize(v, RR(), end);
    }
  }

  template<typename P>
  P serialize( const T& , fas::empty_list, P end)
  {
    return end;
  }


  template<typename LL, typename RR, typename P>
  void deserialize( T& v, fas::type_list<LL, RR>, P beg, P end)
  {
    P first = beg;
    const char *pstr = LL()();
    for ( ; beg!=end && *pstr!='\0' && *pstr==*beg; ++beg, ++pstr);
    if ( beg==end && *pstr=='\0')
    {
      v = LL::value;
    }
    else
      deserialize(v, RR(), first, end);
  }

  template<typename P>
  void deserialize( T& , fas::empty_list,P,P)
  {
  }
};

template<typename T, typename L, char Sep>
class serializerT< flags_enumerator<T, L, Sep> >
{
  typedef typename flags_enumerator<T, L>::enum_list enum_list;
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

    /*
    if (*beg =='"') 
    {
      ++beg;
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      P first = beg;
      for ( ; beg!=end && *beg!='"'; ++beg);
      if (beg==end)
        return create_error<error_code::UnexpectedEndFragment>(e, end);

      if (*beg!='"') 
        return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );

      this->deserialize(v, enum_list(), first, beg);
      ++beg;
    } 
    else if (*beg == '[') 
    {
      ++beg;
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      beg = this->deserialize_arr(v, enum_list(), beg, end, e);
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if (*beg!=']') 
        return create_error<error_code::ExpectedOf>(e, end, "]", std::distance(beg, end) );

      ++beg;
    } 
    else
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );
    */

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
      //beg = parser::parse_space(beg, end, e);
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
      /*if ( *beg!=fin && *beg!=Sep )
        return create_error<error_code::ExpectedOf>(e, end, (Sep==',' ? "]" : "\""), std::distance(beg, end) );
        */
      //beg = parser::parse_space(++beg, end, e);
    }
    return beg;
    
    //enum_list(),
    /*
    if ( Sep == ',' )
      beg = parser::parse_space(beg, end, e);

    if ( beg == end )
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( Sep == ',' )
    {
      if ( *beg!='"' )
        return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );
      if ( *beg==']' )
        return beg;
    }
    else if (*beg=='"')
      return beg;

    P first = beg;
    const char *pstr = LL()();
    for ( ; beg!=end && *pstr!='\0' && *pstr==*beg; ++beg, ++pstr);

    if ( beg == end )
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    bool ready = false;
    if ( *pstr=='\0' )
    {
      if ( Sep==',' )
      {
        ready = true;
        beg = parser::parse_space(beg, end, e);
      };
    }
    */
  }

  /*
  template<typename LL, typename RR, typename P>
  P deserialize_one( T& v, fas::type_list<LL, RR>, P beg, P end)
  {
    P first = beg;
    const char *pstr = LL()();
    for ( ; beg!=end && *pstr!='\0' && *pstr==*beg; ++beg, ++pstr);

    if ( beg==end && *pstr=='\0') 
    {
      v = static_cast<T>(v|LL::value);
      return end;
    } 
    else if (*beg==Sep && *pstr=='\0') 
    {
      beg++;
      v = static_cast<T>(v|LL::value);
      return beg;
    }
    return deserialize_one(v, RR(), first, end);
  }

  template<typename P>
  P deserialize_one( T& , fas::empty_list, P beg, P )
  {
    return beg;
  }

  template<typename LL, typename RR, typename P>
  void deserialize( T& v, fas::type_list<LL, RR> tl, P beg, P end) 
  {
    v = T();
    P beg_prev = end;
    while (beg != beg_prev && beg!=end) 
    {
      beg_prev = beg;
      beg = deserialize_one(v, tl, beg, end);
    }
  }

  template<typename LL, typename RR, typename P>
  P deserialize_arr( T& v, fas::type_list<LL, RR>, P beg, P end, json_error* e) 
  {
    v = T();
    char lastChar = ' ';
    while ( *beg=='"' )
    {
      ++beg;
      if (beg==end)
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      
      P first = beg;
      for ( ; beg!=end && *beg!='"'; ++beg);
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);

      if (*beg!='"') 
        return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );

      this->deserialize_one(v, enum_list(), first, beg);
      beg++;
      lastChar = *beg;
      if (lastChar==',')
        beg++;
    }

    if (lastChar==',') 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    return beg;
  }
  */
};


}}

