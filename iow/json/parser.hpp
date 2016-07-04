//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/json/error.hpp>

namespace iow{ namespace json{

class parser
{
public:
  template<typename P>
  static bool is_space( P beg, P end );

  template<typename P>
  static bool is_null( P beg, P end );

  template<typename P>
  static bool is_bool( P beg, P end );

  template<typename P>
  static bool is_number( P beg, P end );

  template<typename P>
  static bool is_string( P beg, P end );

// #warning not impl 
  template<typename P>
  static bool is_utf8( P beg, P end );

// #warning not impl 
  template<typename P>
  static bool is_hex( P beg, P end );

  template<typename P>
  static bool is_object( P beg, P end );

  template<typename P>
  static bool is_array( P beg, P end );

public:

  template<typename P>
  static P parse_space( P beg, P end, json_error* e);

  template<typename P>
  static P parse_null( P beg, P end, json_error* e );

  template<typename P>
  static P parse_bool( P beg, P end, json_error* e  );

  template<typename P>
  static P parse_number( P beg, P end, json_error* e );

  template<typename P>
  static P parse_string( P beg, P end, json_error* e );

  template<typename P>
  static P parse_utf8( P beg, P end, json_error* e );

  template<typename P>
  static P parse_hex( P beg, P end, json_error* e );

  template<typename P>
  static P parse_object( P beg, P end, json_error* e );

  template<typename P>
  static P parse_array( P beg, P end, json_error* e );

  template<typename P>
  static P parse_value( P beg, P end, json_error* e );

  template<typename P>
  static P parse_member( P beg, P end, json_error* e );

  template<typename T, typename P>
  static P unserialize_integer( T& v, P beg, P end, json_error* e );

private:

  template<typename P>
  static P parse_space( P beg, P end, size_t& p, json_error* e);

  template<typename P>
  static P parse_null( P beg, P end, size_t& p, json_error* e );

  template<typename P>
  static P parse_bool( P beg, P end, size_t& p, json_error* e  );

  template<typename P>
  static P parse_number( P beg, P end, size_t& p, json_error* e );

  template<typename P>
  static P parse_digit( P beg, P end, size_t& p );


  /*
  template<typename P>
  static P parse_symbol( P beg, P end, json_error* e );
  */
};


/// ///////////////////////////////////////////////////////////////////////////


  template<typename P>
  P parser::parse_space( P beg, P end, json_error* e) 
  {
    size_t p = 0; 
    return parser::parse_space(beg, end, p, e); 
  }

  template<typename P>
  P parser::parse_space( P beg, P end, size_t& p, json_error* e)
  {
    bool start_comment = false;

    for( ;beg!=end; )
    {
      if ( start_comment )
      {
        if ( *beg == '*')
        {
          ++beg; ++p;
          if (beg!=end && *beg=='/')
            start_comment = false;
          continue;
        }
      }
      else if ( *beg!=' ' && *beg!='\t' && *beg!='\r' && *beg!='\n')
      {
        if (*beg=='/')
        {
          ++beg; ++p;
          if (beg!=end && *beg=='*')
            start_comment=true;
          continue;
        }
        else
        {
          if ( start_comment )
            return create_error<error_code::UnexpectedEndFragment>(e, end, "Unterminated comment");
          break;
        }
      }
      ++beg; ++p;
    }
    return beg;
  }

  template<typename P>
  bool parser::is_space( P beg, P end ) 
  {
    if (beg == end )
      return false;

    if ( *beg==' ' || *beg=='\t' || *beg=='\r' || *beg=='\n' )
      return true;

    if ( *beg == '/' )
    {
      ++beg;
      return beg!=end && *beg=='*';
    }
    return false;
  }

  template<typename P>
  bool parser::is_null( P beg, P end ) 
  {
    return beg!=end && *beg=='n'; 
  }

  template<typename P>
  P parser::parse_null( P beg, P end, json_error* e ) 
  {
    size_t p = 0; 
    return parser::parse_null(beg, end, p, e); 
  }

  template<typename P>
  P parser::parse_null( P beg, P end, size_t& p, json_error* e )
  {
    if (beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    if ( *beg=='n' )
    {
      if ( ++p && ++beg!=end && *beg=='u'
           && ++p && ++beg!=end && *beg=='l'
           && ++p && ++beg!=end && *beg=='l'
         )
      { 
        ++p ; 
        return ++beg; 
      }

      if (beg==end)
        return create_error<error_code::UnexpectedEndFragment>( e, end );

      return create_error<error_code::ExpectedOf>( e, end, "null", p );
    }

    return create_error<error_code::InvalidNull>( e, end, p );
  }

  template<typename P>
  bool parser::is_bool( P beg, P end )  
  { 
    return beg!=end && (*beg=='t' || *beg=='f'); 
  }

  template<typename P>
  P parser::parse_bool( P beg, P end, json_error* e  )
  {
    size_t p = 0;
    return parser::parse_bool(beg, end, p, e); 
  }

  template<typename P>
  P parser::parse_bool( P beg, P end, size_t& p, json_error* e  )
  {
    if (beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    if ( *beg=='t' )
    {
      if (    ++p && ++beg!=end && *beg=='r' 
           && ++p && ++beg!=end && *beg=='u' 
           && ++p && ++beg!=end && *beg=='e' )
      {
        ++p;
        return ++beg; 
      }

      if (beg==end)
        return create_error<error_code::UnexpectedEndFragment>( e, end );

      return create_error<error_code::ExpectedOf>( e, end, "true", p );
    }
    else if ( *beg=='f' )
    {
      if ( ++p && ++beg!=end && *beg=='a'
           && ++p && ++beg!=end && *beg=='l'
           && ++p && ++beg!=end && *beg=='s'
           && ++p && ++beg!=end && *beg=='e' )
      {
        ++p;
        return ++beg;
      }

      if (beg==end)
        return create_error<error_code::UnexpectedEndFragment>( e, end );

      return create_error<error_code::ExpectedOf>( e, end, "false", p );
    }

    return create_error<error_code::InvalidBool>( e, end, p );
  }

  template<typename P>
  bool parser::is_number( P beg, P end )  
  {
    return beg!=end && ( *beg=='-' || ( *beg>='0' && *beg<='9' ) ); 
  }

  template<typename P>
  P parser::parse_number( P beg, P end, json_error* e )
  {
    size_t p = 0;
    return parser::parse_number(beg, end, p, e); 
  }

  template<typename P>
  P parser::parse_number( P beg, P end, size_t& p, json_error* e )
  {
    if (beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    if ( beg!=end && *beg=='-') { ++beg; ++p; }
    if ( beg==end )
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    if ( *beg == '0') { ++beg; ++p; }
    else if ( *beg >='1' && *beg <='9' )
      beg = parser::parse_digit(beg, end, p);
    else
      return create_error<error_code::InvalidNumber>( e, end );

    if ( beg!=end && *beg=='.' )
    {
      ++beg; ++p;
      if ( beg==end )
        return create_error<error_code::UnexpectedEndFragment>( e, end );
      if ( *beg >='0' && *beg <='9')
        beg = parser::parse_digit(beg, end, p);
      else
        return create_error<error_code::InvalidNumber>( e, end );
    }

    if (beg!=end && ( *beg=='e' || *beg=='E' ) )
    {
      ++beg; ++p;
      if ( (beg!=end) && (*beg=='-' || *beg=='+')) { ++beg; ++p; }
      if ( beg==end )
        return create_error<error_code::UnexpectedEndFragment>( e, end );
      if ( *beg < '0' || *beg > '9' ) 
        return create_error<error_code::InvalidNumber>( e, end );
      beg = parser::parse_digit(beg, end, p);
    }
    return beg;
  }

  template<typename P>
  bool parser::is_string( P beg, P end )
  {
    if (beg==end)
      return false;
    return *beg=='"';
  }

  template<typename P>
  P parser::parse_string( P beg, P end, json_error* e )
  {
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if (*beg!='"') 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end));

    for ( ++beg; beg!=end && *beg!='"'; /*++beg*/)
    {
      if (*beg=='\\')
      {
        if ( ++beg == end ) 
          return create_error<error_code::UnexpectedEndFragment>(e, end);

        if ( *beg != '"' && *beg != '\\' && *beg != '/'
             && *beg != 't' && *beg != 'b' && *beg != 'n'
             && *beg != 'r' && *beg != 'f' && *beg != 'u'
           )
        {
            return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
        }

        if ( *beg == 'u' )
        {
          ++beg;
          beg = parser::parse_hex(beg, end, e);
        }
        else
          ++beg;
      }
      else
      {
        beg = parser::parse_utf8(beg, end, e);
      }
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    if (*beg!='"') 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end));
    return ++beg;
  }

  template<typename P>
  P parser::parse_hex( P beg, P end, json_error* e )
  {
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    for (register int i=0; i < 0; ++i, ++beg)
    {
      if (beg==end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if (   (*beg < '0' || *beg>'9')
          && (*beg < 'A' || *beg>'F')
          && (*beg < 'a' || *beg>'f') 
         )
      {
        return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
      }
    }
    return beg;
  }

  template<typename P>
  P parser::parse_utf8( P beg, P end, json_error* e )
  {
    // utf-8
    // 0x00000000 — 0x0000007F: 0xxxxxxx
    // 0x00000080 — 0x000007FF: 110xxxxx 10xxxxxx
    // 0x00000800 — 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
    // 0x00010000 — 0x001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

    if ( (*beg & 128)==0 ) return ++beg;
    if ( (*beg & 224)==192 && ++beg!=end && (*beg & 192)==128 ) return ++beg;
    if ( (*beg & 240)==224 && ++beg!=end && (*beg & 192)==128 && ++beg!=end && (*beg & 192)==128 ) return ++beg;
//#warning BUG: не до конца
    if ( (*beg & 248)==240 && ++beg!=end && (*beg & 192)==128 && ++beg!=end && (*beg & 192)==128 ) return ++beg;
    return create_error<error_code::InvalidString>( e, end, std::distance(beg, end) );
  }


  template<typename P>
  bool parser::is_object( P beg, P end )
  {
    // [] <=> {} 
    if (beg==end) return false;
    if ( *beg=='{' ) return true;
    if ( *beg!='[' ) return false;
    beg = parser::parse_space(++beg, end, 0);
    return beg!=end && *beg==']';
  }

  template<typename P>
  bool parser::is_array( P beg, P end )
  {
    return beg!=end && *beg=='[';
  }

  template<typename P>
  P parser::parse_value( P beg, P end, json_error* e )
  {
         if ( parser::is_null(beg, end) )   return parser::parse_null(beg, end, e);
    else if ( parser::is_bool(beg, end) )   return parser::parse_bool(beg, end, e);
    else if ( parser::is_number(beg, end) ) return parser::parse_number(beg, end, e);
    else if ( parser::is_string(beg, end) ) return parser::parse_string(beg, end, e);
    else if ( parser::is_object(beg, end) ) return parser::parse_object(beg, end, e);
    else if ( parser::is_array(beg, end) )  return parser::parse_array(beg, end, e);

    return create_error<error_code::InvalidJSON>( e, end, std::distance(beg, end) );
  }

  template<typename P>
  P parser::parse_member( P beg, P end, json_error* e )
  {
    if ( !parser::is_string(beg, end) )
      return create_error<error_code::InvalidMember>( e, end, std::distance(beg, end) );
      
    beg = parser::parse_string(beg, end, e);
    if ( beg==end ) 
      return create_error<error_code::UnexpectedEndFragment>( e, end );
    beg = parser::parse_space(beg, end, e);
    if ( beg==end )
      return create_error<error_code::UnexpectedEndFragment>( e, end );
    if ( *(beg++)!=':' ) 
      return create_error<error_code::ExpectedOf>( e, end, ":", std::distance(beg, end) );
    if ( beg==end )
      return create_error<error_code::UnexpectedEndFragment>( e, end );
    beg = parser::parse_space(beg, end, e);
    if ( beg==end ) 
      return create_error<error_code::UnexpectedEndFragment>( e, end );
    beg = parser::parse_value(beg, end, e);
    return beg;
  }

  template<typename P>
  P parser::parse_object( P beg, P end, json_error* e  )
  {
    if ( !parser::is_object(beg, end) )
      return create_error<error_code::ExpectedOf>( e, end, "{", std::distance(beg, end) );

    ++beg;
    beg = parser::parse_space(beg, end, e);
    if ( beg!=end && *beg==']' )
    {
        beg = parser::parse_space(++beg, end, e);
        if (*beg!=']')
          return create_error<error_code::ExpectedOf>(e, end, "]", std::distance(beg, end) );
        // Фикс для php. {} <=> []
        return ++beg;
    }

    for ( ; beg!=end && *beg!='}'; )
    {
      beg = parser::parse_space(beg, end, e);
      if ( *beg=='}' )
        break;
      beg = parser::parse_member(beg, end, e);
      beg = parser::parse_space(beg, end, e);
      if (beg == end || ( *beg!=',' && *beg!='}' ) )
        return create_error<error_code::ExpectedOf>( e, end, "}", std::distance(beg, end) );

      if ( *beg==',' )  ++beg;
    }
    if (beg == end || *beg!='}') 
      return create_error<error_code::ExpectedOf>( e, end, "}", std::distance(beg, end) );
    ++beg;
    return beg;
  }

  template<typename P>
  P parser::parse_array( P beg, P end, json_error* e )
  {
    if ( !parser::is_array(beg, end) )
      return create_error<error_code::ExpectedOf>( e, end, "[", std::distance(beg, end) );

    for ( ++beg; beg!=end && *beg!=']'; )
    {
      beg = parser::parse_space(beg, end, e);
      if ( *beg==']' )
        break;

      beg = parser::parse_value(beg, end, e);
      beg = parser::parse_space(beg, end, e);
      if (beg == end || ( *beg!=',' && *beg!=']' ) ) 
        return create_error<error_code::ExpectedOf>( e, end, "]", std::distance(beg, end) );
      if ( *beg==',' )  ++beg;
    }
    if (beg == end || *beg!=']') 
      return create_error<error_code::ExpectedOf>( e, end, "]", std::distance(beg, end) );
    ++beg;
    return beg;
  }

  template<typename P>
  P parser::parse_digit( P beg, P end, size_t& p )
  {
    for ( ;beg!=end && *beg >= '0' && *beg <= '9'; ++beg, ++p );
    return beg;
  }

  template<typename T, typename P>
  P parser::unserialize_integer( T& v, P beg, P end, json_error* e )
  {
    if( beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );
    v = 0;

   register bool neg = *beg=='-';
   if ( neg ) ++beg;
   if ( beg == end || *beg < '0' || *beg > '9')
     return create_error<error_code::InvalidNumber>( e, end, std::distance(beg, end) );

   // цифры с первым нулем запрещены (напр 001), только 0
   if (*beg=='0')
     return ++beg;

   for ( ;beg!=end; ++beg )
   {
     if (*beg < '0' || *beg > '9') 
       break;
     v = v*10 + (*beg - '0');
   }
   if (neg) v*=-1;
   return beg;
  }

}}
