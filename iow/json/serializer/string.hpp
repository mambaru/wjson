//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <iow/json/predef.hpp>
#include <iow/json/error.hpp>
#include <iow/json/parser.hpp>

#include <fas/integral/bool_.hpp>

#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>

#include <iostream>
namespace iow{ namespace json{

template<typename T>
class serializerS;

template<>
class serializerS<char>
{
public:
  template<typename P1,typename P>
  P1 serialize_hex(P1 beg, P1 end, P itr)
  {
    // \uFFFF
    std::stringstream ss;
    const size_t bufsize = 8;
    char buf[bufsize]={'\0'};
    ss.rdbuf()->pubsetbuf(buf, bufsize);
    ss << "\\u" << std::setfill('0') << std::setw(4) << std::hex;
    //ss.precision(4);
    //ss.setf ( std::ios::hex, std::ios::basefield );  // set hex as the basefield

    if ( parser::is_utf8(beg+1, beg+2) || beg+1==end)
      ss << static_cast<unsigned short>( static_cast<unsigned char>(*(beg++)) );
    else
    {
      ss << *reinterpret_cast<const unsigned short*>( &*beg );
      beg+=2;
    }
    /*
    if ( parser::is_utf8(beg+1, beg+2) ) ss << "00";
    else ss << std::hex << static_cast<unsigned char>( *(beg++) );
    ss << std::hex << static_cast<unsigned short>( *(beg++) );
    */
    for (int i=0; i < 6; ++i)
      *(itr++) = buf[i];
    return beg;

    /*
    *(itr++) = '\\';
    *(itr++) = 'u';
    char buf[5] = {0};
    std::snprintf(buf, 5, "%04x", static_cast<unsigned short>(*beg) );
    std::cout << "-----hex----" << buf << ": " << int(*beg) << std::endl; 
    for (int i=0; i < 4; ++i)
      *(itr++) = buf[i];
    return ++beg;
    */
  }

  
  
  template<typename P1, typename P>
  P serialize(P1 beg, P1 end, P itr)
  {
    *(itr++)='"';
    for (;beg!=end /*&& *beg!='\0'*/;/*++beg*/)
    {
      if ( static_cast<unsigned char>(*beg) >=32 && static_cast<unsigned char>(*beg) < 127 )
      {
        //std::cout << "ascii: " << *beg << ":" << int(*beg) <<  std::endl;
        // ascii
        *(itr++) = *(beg++);
      }
      else if ( static_cast<unsigned char>(*beg) < 32 ) 
      {
        //std::cout << "space: " << *beg << ":" << int(*beg) <<  std::endl;
        // space
        switch (*beg)
        {
          case '"' :
          case '\\':
          case '/' :  *(itr++)='\\'; *(itr++) = *beg; break;
          case '\t':  *(itr++)='\\'; *(itr++) = 't'; break;
          case '\b':  *(itr++)='\\'; *(itr++) = 'b'; break;
          case '\r':  *(itr++)='\\'; *(itr++) = 'r'; break;
          case '\n':  *(itr++)='\\'; *(itr++) = 'n'; break;
          case '\f':  *(itr++)='\\'; *(itr++) = 'f'; break;
          default: {
            beg = this->serialize_hex( beg, end, itr );
            continue;
          }
        }
        ++beg;
      } 
      else if ( parser::is_utf8(beg, end) )
      {
        json_error e;
        P1 end8 = parser::parse_utf8(beg, end, &e);
        if ( e )
        {
         // std::cout << "bad: " << *beg << std::endl;
          // bad utf8 as hex
          beg = this->serialize_hex( beg, end, itr );
        }
        else for ( ;beg!=end8; ) 
        {
          //std::cout << "utf8: " << *beg << std::endl;
          // is utf8
          *(itr++) = *(beg++);
        }
      }
      else
      {
        //std::cout << "bin: " << *beg << std::endl;
        // bin as hex
        beg = this->serialize_hex( beg, end, itr );
      }
    }
    *(itr++)='"';
    return itr;
  }

  template<typename P, typename P1>
  P unserialize(P beg, P end, P1 vitr, int n /*= -1*/, json_error* e)
  {
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *(beg++) != '"' ) 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) + 1);

    for ( ;beg!=end && *beg!='"' && n!=0; )
    {
      if (*beg=='\\')
      {
        if (++beg==end) 
          return create_error<error_code::UnexpectedEndFragment>(e, end);
        switch (*beg)
        {
          case '"' :
          case '\\':
          case '/' : *(vitr++) = *beg; ++beg; --n; break;
          case 't':  *(vitr++) = '\t'; ++beg; --n; break;
          case 'b':  *(vitr++) = '\b'; ++beg; --n; break;
          case 'r':  *(vitr++) = '\r'; ++beg; --n; break;
          case 'n':  *(vitr++) = '\n'; ++beg; --n; break;
          case 'f':  *(vitr++) = '\f'; ++beg; --n; break;
          case 'u':  beg = this->unserialize_uhex_(++beg, end, &vitr, n, e); break;
          default:
            return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
        }
      }
      else
        beg = this->unserialize_symbol_(beg, end, &vitr, n, e);
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *(beg++) != '"' ) 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) + 1);

    return beg;
  }

private:

  template<typename P, typename P1>
  P unserialize_symbol_(P beg, P end, P1* vitr, int& n, json_error* e)
  {
    if (beg == end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( (*beg & 128)==0 )
    {
      *((*vitr)++) = *(beg++);
      --n;
    }
    else if ( (*beg & 224)==192 )
      beg = this->symbol_copy_<2>(beg, end, vitr, n, e);
    else if ( (*beg & 240)==224 )
      beg = this->symbol_copy_<3>(beg, end, vitr, n, e);
    else if ( (*beg & 248)==240 )
      beg = this->symbol_copy_<4>(beg, end, vitr, n, e);
    else
      return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
    return beg;
  }

  template<int N, typename P, typename P1>
  P symbol_copy_(P beg, P end, P1* vitr, int& n, json_error* e)
  {
    for (register int i = 0; i < N && n!=0; ++i, --n)
    {
      if (beg == end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      *((*vitr)++) = *(beg++);
    }
    return beg;
  }


  // 0x00000000 — 0x0000007F 	0xxxxxxx
  // 0x00000080 — 0x000007FF 	110xxxxx 10xxxxxx
  // 0x00000800 — 0x0000FFFF 	1110xxxx 10xxxxxx 10xxxxxx
  // 0x00010000 — 0x001FFFFF    11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
  template<typename P, typename P1>
  P unserialize_uhex_(P beg, P end, P1* vitr, int& n, json_error* e)
  {
    unsigned short hex = 0;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->toUShort_(*(beg++), e) << 12;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->toUShort_(*(beg++), e) << 8;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->toUShort_(*(beg++), e) << 4;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->toUShort_(*(beg++), e);

    if ( hex <= 0x007F )
    {
      *((*vitr)++) = static_cast<unsigned char>(hex);
      --n;
    }
    else if ( hex <= 0x07FF )
    {
       *((*vitr)++) = 192 | static_cast<unsigned char>( hex >> 6 );
       --n;
       if ( n==0) 
         return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
       *((*vitr)++) = 128 | ( static_cast<unsigned char>( hex ) & 63 );
       --n;
    }
    else
    {
       *((*vitr)++) = 224 | static_cast<unsigned char>( hex >> 12 );
       --n;
       if ( n==0) 
         return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
       *((*vitr)++) = 128 | ( static_cast<unsigned char>( hex >> 6 ) & 63 );
       --n;
       if ( n==0)
         return create_error<error_code::error_code::InvalidString>(e, end, std::distance(beg, end));
       *((*vitr)++) = 128 | ( static_cast<unsigned char>( hex ) & 63 );
       --n;
    }
    return beg;
  }

  unsigned short toUShort_(unsigned char c, json_error* e)
  {
    if ( c >= '0' && c<='9' ) return c - '0';
    if ( c >= 'a' && c<='f' ) return (c - 'a') + 10;
    if ( c >= 'A' && c<='F' ) return (c - 'A') + 10;
    create_error<error_code::error_code::InvalidString>(e, (char*)(0));
    return 0;
  }
};


template<int N>
class serializerT< value< char[N]> >
  : serializerS<char>
{
public:

  typedef char value_type[N];

  template<typename P>
  P operator()( const value_type& v, P end)
  {
    return serialize( v, v+N, end);
  }

  template<typename P>
  P operator() ( value_type& v, P beg, P end )
  {
    for ( register int i =0 ; i < N; ++i)
      v[i]=0;

    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end);

    return this->unserialize(beg, end, &(v[0]), N);
  }
};


template<int R>
class serializerT< value<std::string, R> >
  : serializerS<char>
{
public:
  template<typename P>
  P operator()( const std::string& v, P end)
  {
    return serialize( v.begin(), v.end(), end);
  }

  template<typename P>
  P operator() ( std::string& v, P beg, P end, json_error* e )
  {
    v.clear();
    v.reserve( (R >= 0) ? R : 64);

    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end, e);
    return unserialize(beg, end, std::back_inserter(v), -1, e);
  }
};

template<int R>
class serializerT< value<std::vector<char>, R > >
  : serializerS<char>
{
public:
  template<typename P>
  P operator()( const std::vector<char>& v, P end)
  {
    return serialize( v.begin(), v.end(), end);
  }

  template<typename P>
  P operator() ( std::vector<char>& v, P beg, P end, json_error* e )
  {
    v.clear();
    v.reserve( (R >= 0) ? R : 64);
    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end);
    return unserialize(beg, end, std::back_inserter(v), e);
  }

};

}}


