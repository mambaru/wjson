#include <sstream>

namespace iow{ namespace json{

namespace detail
{
   // ff 255 3
   // ffff 65535 5
   // ffffffff 4294967295 10
   // ffffffffffffffff 18446744073709551615 20
   // 1 - 3  -> 1*2 + 1/2
   // 2 - 5  -> 2*2 + 2/1
   // 4 - 10 -> 4*2 + 4/2
   // 8 - 20 -> 8*2 + 8/2

  template<typename T, int >
  struct is_signed_integer_base
  {
    enum { result = 1 };
    static bool is_less_zero(T v)
    {
      return v < 0;
    }
  };

  template<typename T>
  struct is_signed_integer_base<T, false>
  {
    enum { result = 0 };
    static bool is_less_zero(T ){ return false; }
  };

  
  template<typename T>
  struct is_signed_integer:
    is_signed_integer_base< T, ( T(-1) < T(1) ) >
  {
  };

  template<typename T>
  struct integer_buffer_size
  {
    enum { result = sizeof(T)*2 + sizeof(T)/2 + sizeof(T)%2 + is_signed_integer<T>::result };
  };

  template<typename T>
  class integer_serializer
  {
//#warning убрать из состояния, сделать бенчмарк
    //char _buf[integer_buffer_size<T>::result];
  public:

    integer_serializer() {}

    template<typename P>
    P serialize(T v, P itr)
    {
      register char buf[integer_buffer_size<T>::result];
      register char *beg = buf;
      register char *end = buf;
      if (v==0)
        *(end++) = '0';
      else
      {
        if ( is_signed_integer<T>::is_less_zero(v) ) 
        {
          *(end++)='-';
          ++beg;
          for( ; v!=0 ; ++end, v/=10) 
            *end = '0' - v%10;
        }
        else
          for( ; v!=0 ; ++end, v/=10) 
            *end = '0' + v%10;
      }
      for ( register char* cur = end ; cur-beg > 1;--cur, ++beg) 
      { *beg ^= *(cur-1); *(cur-1)^=*beg; *beg^=*(cur-1); }

      for (beg = buf; beg!=end; ++beg)
        *(itr++)=*beg;

      return itr;
    }

    template<typename P>
    P unserialize ( T& v, P beg, P end, json_error* e )
    {
      return parser::unserialize_integer(v, beg, end, e);
    }
  };


  template<typename T>
  class serializerN: integer_serializer<T>
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
  
  template<typename T, int R>
  class serializerF
  {
  public:
    template<typename P>
    P operator()( T v, P end)
    {
      std::stringstream ss;
      const size_t bufsize = ( R == -1 ? 20 : 20 + R ) ;
      char buf[bufsize]={'\0'};
      ss.rdbuf()->pubsetbuf(buf, bufsize);
      if ( R == -1 ) 
      {
        ss << std::scientific;
        ss << "S:";
      }
      else
      {
        ss << std::fixed;
        ss.precision(R);
        ss << "P:";
      }

      ss << v ;

      for (int i = 0; i < bufsize && buf[i]!='\0'; ++i)
      {
        *(end++) = buf[i];
      }
      return end;
    }
    
    template<typename P>
    P operator() ( T& v, P beg, P end, json_error* e )
    {
      if( beg==end)
        return json_error::create<unexpected_end_fragment>( e, end );

      if ( parser::is_null(beg, end) )
      {
        v = T();
        return parser::parse_null(beg, end, e);
      }
      
      if ( !parser::is_number(beg, end) )
      {
        return json_error::create<invalid_json_number>( e, end, std::distance(beg, end) );
      }
        

      std::stringstream ss;
      ss.rdbuf()->pubsetbuf( &(*beg), std::distance(beg, end) );
      ss >> v;
      return parser::parse_number(beg, end, e);
    }
  };

}


template<>
class serializerT< value<char> >
  : public detail::serializerN<char>
{
};

template<>
class serializerT< value<unsigned char> >
  : public detail::serializerN<unsigned char>
{
};

template<>
class serializerT< value<short> >
  : public detail::serializerN<short>
{
};

template<>
class serializerT< value<unsigned short> >
  : public detail::serializerN<unsigned short>
{
};

template<>
class serializerT< value<int> >
  : public detail::serializerN<int>
{
};

template<>
class serializerT< value<unsigned int> >
  : public detail::serializerN<unsigned int>
{
};

template<>
class serializerT< value<long> >
  : public detail::serializerN<long>
{
};

template<>
class serializerT< value<unsigned long> >
  : public detail::serializerN<unsigned long>
{
};


template<>
class serializerT< value<long long> >
  : public detail::serializerN<long long>
{
};

template<>
class serializerT< value<unsigned long long> >
  : public detail::serializerN<unsigned long long>
{
};

template<int R>
class serializerT< value<float, R> >
  : public detail::serializerF<float, R>
{
};

template<int R>
class serializerT< value<double, R> >
  : public detail::serializerF<double, R>
{
};

template<int R>
class serializerT< value<long double, R> >
  : public detail::serializerF<long double, R>
{
};


template<>
class serializerT< value<bool> >
{

public:
  template<typename P>
  P operator()( bool v, P beg)
  {
    if ( v ) { *(beg++)='t'; *(beg++)='r';*(beg++)='u';*(beg++)='e'; }
    else { *(beg++)='f'; *(beg++)='a';*(beg++)='l';*(beg++)='s'; *(beg++)='e'; }
    return beg;
  }

  template<typename P>
  P operator() ( bool& v, P beg, P end, json_error* e )
  {
    if (beg==end)
      return json_error::create<unexpected_end_fragment>( e, end );

    if ( parser::is_null(beg, end) )
    {
      v = bool();
      return parser::parse_null(beg, end, e);
    }

    v = *beg == 't';
    if ( v )
      return parser::parse_bool(beg, end, e);

    if ( *beg == 'f' )
      return parser::parse_bool(beg, end, e);

    return json_error::create<invalid_json_bool>( e, end, std::distance(beg, end) );

  }
};

}}
