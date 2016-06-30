#include <memory>
#include <vector>
#include <fas/integral/bool_.hpp>

namespace iow{ namespace json{

template<typename T>
class serializerS;

template<>
class serializerS<char>
{
public:
  template<typename P1,typename P>
  P serialize(P1 beg, P1 end, P itr)
  {
    *(itr++)='"';
    for (;beg!=end && *beg!='\0';++beg)
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
        default:
        {
          if ( *beg>=0 && *beg<32)
          {
            *(itr++) = '\\';
            *(itr++) = 'u';
            char buf[5] = {0};
            snprintf(buf, 5, "%04x", int(*beg) );
            for (int i=0; i < 4; ++i)
              *(itr++) = buf[i];
          }
          else
            *(itr++) = *beg;
        }
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
          case 'u':  beg = _unserialize_uhex(++beg, end, &vitr, n, e); break;
          default:
            return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
        }
      }
      else
        beg = _unserialize_symbol(beg, end, &vitr, n, e);
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *(beg++) != '"' ) 
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) + 1);

    return beg;
  }

private:

  template<typename P, typename P1>
  P _unserialize_symbol(P beg, P end, P1* vitr, int& n, json_error* e)
  {
    if (beg == end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( (*beg & 128)==0 )
    {
      *((*vitr)++) = *(beg++);
      --n;
    }
    else if ( (*beg & 224)==192 )
      beg = _symbol_copy<2>(beg, end, vitr, n, e);
    else if ( (*beg & 240)==224 )
      beg = _symbol_copy<3>(beg, end, vitr, n, e);
    else if ( (*beg & 248)==240 )
      beg = _symbol_copy<4>(beg, end, vitr, n, e);
    else
      return create_error<error_code::InvalidString>(e, end, std::distance(beg, end));
    return beg;
  }

  template<int N, typename P, typename P1>
  P _symbol_copy(P beg, P end, P1* vitr, int& n, json_error* e)
  {
    for (register int i = 0; i < N && n!=0; ++i, --n)
    {
      if (beg == end) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      *((*vitr)++) = *(beg++);
    }
    return beg;
  }

/*
0x00000000 — 0x0000007F 	0xxxxxxx
0x00000080 — 0x000007FF 	110xxxxx 10xxxxxx
0x00000800 — 0x0000FFFF 	1110xxxx 10xxxxxx 10xxxxxx

U+0439	й	d0 b9	CYRILLIC SMALL LETTER SHORT I
00000100 00111001 -> 110_10000 10_111001

// উ - U+0989 - 	e0 a6 89
// 00001001 10001001 -> 11100000 10100110 10001001
*/
  template<typename P, typename P1>
  P _unserialize_uhex(P beg, P end, P1* vitr, int& n, json_error* e)
  {
    unsigned short hex = 0;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= _toUShort(*(beg++), e) << 12;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= _toUShort(*(beg++), e) << 8;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= _toUShort(*(beg++), e) << 4;
    if (beg == end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);
    hex |= _toUShort(*(beg++), e);

    if ( hex <= 0x007F )
    {
      *((*vitr)++) = static_cast<unsigned char>(hex);
      --n;
    }
    else if ( hex <= 0x007FF )
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

  unsigned short _toUShort(unsigned char c, json_error* e)
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
    //this->reserve( v, fas::bool_< (R>0) >() );
    v.reserve( (R >= 0) ? R : 64);

    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end, e);
    return unserialize(beg, end, std::back_inserter(v), -1, e);
  }
 /* 
  void reserve( std::string&, fas::false_){}

  
  void reserve( std::string& v, fas::true_)
  {
    v.reserve(R);
  }
  */

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
    //this->reserve( v, fas::bool_< (R>0) >() );
    if ( parser::is_null(beg, end) )
      return parser::parse_null(beg, end);
    return unserialize(beg, end, std::back_inserter(v), e);
  }

  /*
  void reserve( std::vector<char>&, fas::false_){}

  void reserve( std::vector<char>& v, fas::true_)
  {
    v.reserve(R);
  }
  */
};

template<typename T>
class serializerT< iterator_pair<T> >
{
public:
  template<typename P>
  P operator()( const T& v, P end )
  {
    if ( v.first != v.second )
      return std::copy(v.first, v.second, end );
    else
    {
      *(end++)='"';
      *(end++)='"';
      return end;
    }
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    v.first = beg;
    beg = parser::parse_value(beg, end, e);
    v.second = beg;
    return beg;
  }
};


template<typename T, int R>
class serializerT< raw_value<T, R> >
{
public:
  template<typename P>
  P operator()( const T& v, P end)
  {
    if ( v.begin() != v.end() )
    {
      return std::copy(v.begin(), v.end(), end );
    }
    else
    {
      *(end++)='"';
      *(end++)='"';
      return end;
    }
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    v.clear();
    P start = beg;
    beg = parser::parse_value(beg, end, e);
    std::copy( start, beg, std::back_inserter(v) );
    return beg;
  }
};

template<typename T, typename J>
class serializerT< pointer<T, J> >
{
  //typedef typename T target;
public:
  template< typename P>
  P operator()( const T& ptr, P end)
  {
    // Можно обычный указатель
    if ( ptr!=0 )
      return typename J::serializer()( *ptr, end);
    
    *(++end)='n';
    *(++end)='u';
    *(++end)='l';
    *(++end)='l';
    return end;
  }

  template< typename Type, typename P>
  P operator() ( std::unique_ptr<Type>& ptr, P beg, P end, json_error* e )
  {
    // Только умный
    if (beg!=end && *beg!='n')
    {
      ptr = std::unique_ptr<Type>(new Type());
      return typename J::serializer()( *ptr, beg, end, e);
    }
    else
    {
      ptr=0;
      for (int i=0; beg!=end && i<4; ++i, ++beg);
    }
    return beg;
  }
  
  template< typename Type, typename P>
  P operator() ( std::shared_ptr<Type>& ptr, P beg, P end, json_error* e )
  {
    // Только умный
    if (beg!=end && *beg!='n')
    {
      ptr = std::make_shared<Type>();
      return typename J::serializer()( *ptr, beg, end, e);
    }
    else
    {
      ptr=0;
      for (int i=0; beg!=end && i<4; ++i, ++beg);
    }
    return beg;
  }
};



}}


