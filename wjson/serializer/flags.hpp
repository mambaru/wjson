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

template<typename T, typename L, char Sep, typename Mode>
class serializerT< flags<T, L, Sep, Mode> >
{
  typedef typename flags<T, L>::enum_list enum_list;
  typedef serializerT< flags<T, L, Sep, Mode> > self;
public:
  template<typename P>
  P operator()( const T& v, P end) const
  {
    *(end++)= ( Sep == ',' ? '[' : '"' );
    end = self::serialize(v, enum_list(), end,true);
    *(end++)=( Sep == ',' ? ']' : '"' );
    return end;
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e ) const
  {
    v = T();
    
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    bool as_array = false;
    if ( *beg == '[' )
    {
      as_array = true;
    }
    else if ( *beg != '"' )
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );

    ++beg;

    if ( as_array )
      beg = self::deserialize<true>(v, beg, end, e);
    else
      beg = self::deserialize<false>(v, beg, end, e);

    if ( beg==end )
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *beg != ']' && *beg!='"'  )
      return create_error<error_code::ExpectedOf>(e, end, "]", std::distance(beg, end) );
    ++beg;

    return beg;
  }

private:

  template<typename LL, typename RR, typename P>
  static P serialize( const T& v, fas::type_list<LL, RR>, P end, bool isFirst)
  {
    if ( ( (LL::value!=static_cast<T>(0)) && (LL::value & v) == LL::value) || (v == LL::value) )
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

      return self::serialize(v, RR(), end, false);
    }
    else
    {
      return self::serialize(v, RR(), end, isFirst);
    }
  }

  template<typename P>
  static P serialize( const T&, fas::empty_list, P end, bool)
  {
    return end;
  }

  // string with separator
  template<typename LL, typename RR, typename P>
  static P deserialize_one( T& v, P beg, P end, json_error* e, fas::type_list<LL, RR>, fas::false_, bool ready )
  {
    if ( beg==end ) 
      return beg;

    // Если разделитель не пробел, то разрешаем пробелы
    if ( Sep!=' ' )
    {
      beg = parser::parse_space(beg, end, e);
      if ( beg == end )
        return beg;
    }
      
    if ( *beg=='"' )
      return beg;
    
    P cur = beg;
    const char *pstr = LL()();

    // парсим текущее значение
    for ( ; cur!=end && *pstr!='\0' && *pstr==*cur; ++cur, ++pstr);

    // если совпало 
    if ( *pstr == '\0' && cur!=end )
    {
      if ( (Sep!=' ' && parser::is_space(cur, end)) || *cur==Sep || *cur=='"' )
      {
        ready = true;
        v = static_cast<T>(v|LL::value);
        beg = cur;
        // Если разделитель не пробел, то разрешаем пробелы
        if ( Sep!=' ' )
        {
          beg = parser::parse_space(beg, end, e);
          if ( beg == end )
            return beg;
        }
          
        if ( *beg=='"' )
          return beg;
        
        if ( *beg==Sep )
          ++beg;
      }
    }
    return self::deserialize_one(v, beg, end, e, RR(), fas::false_(), ready );
  }

  // enum as json array
  template<typename LL, typename RR, typename P>
  static P deserialize_one( T& v, P beg, P end, json_error* e, fas::type_list<LL, RR>, fas::true_, bool ready )
  {
    beg = parser::parse_space(beg, end, e);
    if ( beg==end ) return beg;
    if ( *beg==']' )
      return beg;

    if ( *beg!='"' )
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );;

    P cur = beg;
    ++cur;
   
    const char *pstr = LL()();
    for ( ; cur!=end && *pstr!='\0' && *pstr==*cur; ++cur, ++pstr);

    if ( *pstr == '\0' && cur!=end )
    {
      if ( *cur=='"' )
      {
        ready = true;
        v = static_cast<T>(v|LL::value);
        beg = cur;
        ++beg;
        beg = parser::parse_space(beg, end, e);
        if ( beg == end )
          return beg;
        if ( *beg != ',' )
          return beg;
        ++beg;
        beg = parser::parse_space(beg, end, e);
      }
    }
    return self::deserialize_one(v, beg, end, e, RR(), fas::true_(), ready );
  }

  template<typename P>
  static P deserialize_one( T&, P beg, P end, json_error* e, fas::empty_list, fas::true_, bool ready)
  {
    if ( ready || fas::same_type<Mode, strict_mode>::value  )
      return beg;

    // parse unknown value
    beg = parser::parse_space(beg, end, e);
    if ( *beg==']' ) return beg;
    beg = parser::parse_string(beg, end, e);
    beg = parser::parse_space(beg, end, e);
    if ( beg == end ) return beg;
    if ( *beg == ',' ) ++beg;
    return parser::parse_space(beg, end, e);
  }
  
  template<typename P>
  static P deserialize_one( T&, P beg, P end, json_error* e, fas::empty_list, fas::false_, bool ready)
  {
    if ( ready || fas::same_type<Mode, strict_mode>::value  )
      return beg;
    
    beg = parser::parse_space(beg, end, e);
    for ( ; beg!=end && *beg!=Sep && *beg!='"'; ++beg);
    beg = parser::parse_space(beg, end, e);
    if ( beg == end ) return beg;
    if ( *beg == Sep ) ++beg;
    return parser::parse_space(beg, end, e);
  }
  

  template<bool IsArray, typename P>
  static P deserialize( T& v, P beg, P end, json_error* e) 
  {
    //const char fin = ( Sep2 == ',' ) ? ']' : '"';
    const char fin = IsArray ? ']' : '"';
    while ( beg!=end && *beg!=fin )
    {
      P cur = beg;
      beg = self::deserialize_one(v, beg, end, e, enum_list(), fas::bool_<IsArray>(), false );
      if ( cur == beg )
        return create_error<error_code::InvalidEnum>(e, end, std::distance(beg, end));
      
      if ( beg == end )
        return create_error<error_code::UnexpectedEndFragment>(e, end);
    }
    return beg;
  }
};

}

