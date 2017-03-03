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

#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>

namespace wjson{

template<typename T, bool NullTerm>
class serializerS;

template<bool NullTerm>
class serializerS<char, NullTerm>
{
public:
  template<typename P1, typename P>
  P serialize(P1 beg, P1 end, P itr)
  {
    *(itr++)='"';

    for (;beg!=end && ( !NullTerm || *beg!='\0' ) ;)
    {
      if ( static_cast<unsigned char>(*beg) < 32 || *beg=='"' || *beg=='\\' || *beg=='/' ) 
      {
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
            beg = this->serialize_hex_( beg, itr );
            continue;
          }
        }
        ++beg;
      } 
      else if ( static_cast<unsigned char>(*beg) >=32 && static_cast<unsigned char>(*beg) < 127 )
      {
        *(itr++) = *(beg++);
      }
      else if ( parser::is_utf8(beg, end) )
      {
        json_error e;
        P1 end8 = parser::parse_utf8(beg, end, &e);
        if ( e )
        {
          beg = this->serialize_hex_( beg, itr );
        }
        else for ( ;beg!=end8; ) 
        {
          *(itr++) = *(beg++);
        }
      }
      else
      {
        beg = this->serialize_hex_( beg, itr );
      }
    }
    *(itr++)='"';
    return itr;
  }

  template<typename P, typename P1>
  P unserialize(P beg, P end, P1 vitr, int n , json_error* e)
  {
    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *(beg++) != '"' ) 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) + 1);

    for ( ;beg!=end && *beg!='"' && n!=0 ; )
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
          case 'x':  beg = this->unserialize_xhex_(++beg, end, &vitr, n, e); break;
          default:
            return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
        }
      }
      else
        beg = this->unserialize_utf8_(beg, end, &vitr, n, e);
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *(beg++) != '"' ) 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) + 1);

    return beg;
  }

private:

  template<typename P1,typename P>
  P1 serialize_hex_(P1 beg, P& itr)
  {
    std::stringstream ss;
    const size_t bufsize = 8;
    char buf[bufsize]={'\0'};
    ss.rdbuf()->pubsetbuf(buf, bufsize);
    if ( static_cast<unsigned char>(*beg) < 128 )
      ss << "\\u" << std::setw(4) << std::setfill('0') << std::hex << static_cast<unsigned short>(*beg);
    else
      ss << "\\x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned int>(static_cast<unsigned char>(*beg));

    for (size_t i=0; i < bufsize && buf[i]!='\0'; ++i)
      *(itr++) = buf[i];
    return ++beg;
  }

  template<typename P, typename P1>
  P unserialize_utf8_(P beg, P end, P1* vitr, int& n, json_error* e)
  {
    if (beg == end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( (*beg & 128)==0 )
    {
      *((*vitr)++) = *(beg++);
      --n;
    }
    else if ( (*beg & 224)==192 )
      beg = this->utf8_copy_<2>(beg, end, vitr, n, e);
    else if ( (*beg & 240)==224 )
      beg = this->utf8_copy_<3>(beg, end, vitr, n, e);
    else if ( (*beg & 248)==240 )
      beg = this->utf8_copy_<4>(beg, end, vitr, n, e);
    else
      return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
    return beg;
  }

  template<int N, typename P, typename P1>
  P utf8_copy_(P beg, P end, P1* vitr, int& n, json_error* e)
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
    if ( n==0 ) return beg;
    P cur = beg;
    unsigned short hex = 0;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->uchar2_<unsigned short>( static_cast<unsigned char>(*(beg++)), e) << 12;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->uchar2_<unsigned short>( static_cast<unsigned char>(*(beg++)), e) << 8;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->uchar2_<unsigned short>( static_cast<unsigned char>(*(beg++)), e) << 4;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->uchar2_<unsigned short>( static_cast<unsigned char>(*(beg++)), e);

    if ( hex < 32 )
    {
      if ( hex=='\t' || hex== '\r' || hex== '\n' || hex== '\b' || hex== '\f' )
      {
        *((*vitr)++) = static_cast<char>(hex);
        --n;
      }
      else
      {
        *((*vitr)++) = '\\';
        --n;
        if ( n==0 ) 
          return beg;
        *((*vitr)++) = 'u';
        --n;
        if ( n== 0 ) 
          return beg;
        for (; cur!=beg ; ++cur)
        {
          --n;
          *((*vitr)++) = *cur;
          if ( n == 0 ) 
            return beg;
        }
      }
    }
    else if ( hex <= 0x007F )
    {
      *((*vitr)++) = static_cast<char>(hex);
      --n;
    }
    else if ( hex <= 0x07FF )
    {
       *((*vitr)++) = static_cast<char>(192 | static_cast<unsigned char>( hex >> 6 ));
       --n;
       if ( n==0 )
         return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
       *((*vitr)++) = static_cast<char>( 128 | ( static_cast<unsigned char>( hex ) & 63 ) );
       --n;
    }
    else
    {
       *((*vitr)++) = static_cast<char>( 224 | static_cast<unsigned char>( hex >> 12 ) );
       --n;
       if ( n==0) 
         return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
       *((*vitr)++) = static_cast<char>( 128 | ( static_cast<unsigned char>( hex >> 6 ) & 63 ) );
       --n;
       if ( n==0)
         return create_error<error_code::error_code::InvalidString>(e, end, std::distance(beg, end));
       *((*vitr)++) = static_cast<char>( 128 | ( static_cast<unsigned char>( hex ) & 63 ) );
       --n;
    }
    return beg;
  }

  template<typename P, typename P1>
  P unserialize_xhex_(P beg, P end, P1* vitr, int& n, json_error* e)
  {
    unsigned char hex = 0;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex = this->uchar2_<unsigned char>( static_cast<unsigned char>(*(beg++)), e) << 4;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= this->uchar2_<unsigned char>( static_cast<unsigned char>(*(beg++)), e);
    *((*vitr)++) = static_cast<char>(hex);
    --n;
    return beg;
  }

  template<typename Res>
  Res uchar2_(unsigned char c, json_error* e)
  {
    if ( c >= '0' && c<='9' ) return static_cast<Res>(  c - '0');
    if ( c >= 'a' && c<='f' ) return static_cast<Res>( (c - 'a') + 10 );
    if ( c >= 'A' && c<='F' ) return static_cast<Res>( (c - 'A') + 10 );
    create_error<error_code::error_code::InvalidString>(e, static_cast<char*>(0));
    return static_cast<Res>(0);
  }
};


template<int N>
class serializerT< value< char[N]> >
  : serializerS<char, true>
{
public:
  typedef char value_type[N];

  template<typename P>
  P operator()( const value_type& v, P end)
  {
    return this->serialize( v, v+N, end);
  }

  template<typename P>
  P operator() ( value_type& v, P beg, P end, json_error* e )
  {
    for ( register int i =0 ; i < N; ++i)
      v[i]=0;

    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end, e);

    return this->unserialize(beg, end, &(v[0]), N, e);
  }
};


template<int R>
class serializerT< value<std::string, R> >
  : serializerS<char, false>
{
public:
  template<typename P>
  P operator()( const std::string& v, P end)
  {
    return this->serialize( v.begin(), v.end(), end);
  }

  template<typename P>
  P operator() ( std::string& v, P beg, P end, json_error* e )
  {
    v.clear();
    v.reserve( (R >= 0) ? R : 64);

    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end, e);
    return this->unserialize(beg, end, std::back_inserter(v), std::numeric_limits<int>::max(), e);
  }
};

template<int R>
class serializerT< value<std::vector<char>, R > >
  : serializerS<char, false>
{
public:
  template<typename P>
  P operator()( const std::vector<char>& v, P end)
  {
    return this->serialize( v.begin(), v.end(), end);
  }

  template<typename P>
  P operator() ( std::vector<char>& v, P beg, P end, json_error* e )
  {
    v.clear();
    v.reserve( (R >= 0) ? R : 64);
    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end, e);
    return this->unserialize(beg, end, std::back_inserter(v), std::numeric_limits<int>::max(), e);
  }
};

}


