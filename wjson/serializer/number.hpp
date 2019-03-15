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

namespace detail
{
  template<typename T, int >
  struct is_signed_integer_base
  {
    enum { value = 1 };
    static bool is_less_zero(T v) { return v < 0; }
  };

  template<typename T>
  struct is_signed_integer_base<T, false>
  {
    enum { value = 0 };
    static bool is_less_zero(T ) { return false; }
  };

  template<typename T>
  struct is_signed_integer:
    is_signed_integer_base< T, ( T(-1) < T(1) ) >
  {
  };

  template<typename T>
  struct integer_buffer_size
  {
    // ff 255 3
    // ffff 65535 5
    // ffffffff 4294967295 10
    // ffffffffffffffff 18446744073709551615 20
    // 1 - 3  -> 1*2 + 1%2
    // 2 - 5  -> 2*2 + 2/1
    // 4 - 10 -> 4*2 + 4/2
    // 8 - 20 -> 8*2 + 8/2
    // 16 - 40 -> 16*2 + 16/2
    enum 
    {
      value = sizeof(T)==1 
        ? 4 
        : sizeof(T)*2 + sizeof(T)/2 + sizeof(T)%2 + is_signed_integer<T>::value 
    };
  };

  template<typename T>
  class integer_serializer
  {
  public:

    integer_serializer() {}

    template<typename P>
    static P serialize(T v, P itr)
    {
      char buf[integer_buffer_size<T>::value];
      char *beg = buf;
      char *end = buf;
      if ( v==0 )
        *(end++) = '0';
      else
      {
        if ( is_signed_integer<T>::is_less_zero(v) ) 
        {
          for( ; v!=0 ; ++end, v/=10) 
            *end = static_cast<char>( '0' - static_cast<char>(v%10) );
          *(end++)='-';
        }
        else
        {
          for( ; v!=0 ; ++end, v/=10) 
            *end = static_cast<char>( '0' + static_cast<char>(v%10) );
        }
      }

      do { *(itr++)=*(--end); } while( end != beg );

      return itr;
    }

    template<typename P>
    static P unserialize ( T& v, P beg, P end, json_error* e )
    {
      return parser::unserialize_integer(v, beg, end, e);
    }
  };


}

template<typename T>
class serializerN: detail::integer_serializer<T>
{
public:
  template<typename P>
  P operator()( T v, P end)
  {
     return this->serialize(v, end);
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
     if ( parser::is_null(beg, end) )
     {
       v = T();
       return parser::parse_null(beg, end, e);
     }

     return this->unserialize(v, beg, end, e);
  }
};


template<>
class serializerT< value<char> >
  : public serializerN<char>
{
};

template<>
class serializerT< value<unsigned char> >
  : public serializerN<unsigned char>
{
};

template<>
class serializerT< value<short> >
  : public serializerN<short>
{
};

template<>
class serializerT< value<unsigned short> >
  : public serializerN<unsigned short>
{
};

template<>
class serializerT< value<int> >
  : public serializerN<int>
{
};

template<>
class serializerT< value<unsigned int> >
  : public serializerN<unsigned int>
{
};

template<>
class serializerT< value<long> >
  : public serializerN<long>
{
};

template<>
class serializerT< value<unsigned long> >
  : public serializerN<unsigned long>
{
};

template<>
class serializerT< value<long long> >
  : public serializerN<long long>
{
};

template<>
class serializerT< value<unsigned long long> >
  : public serializerN<unsigned long long>
{
};

}
