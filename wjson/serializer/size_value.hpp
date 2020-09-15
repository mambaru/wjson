//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/error.hpp>
#include <wjson/parser.hpp>
#include <fas/integral.hpp>
#include <fas/system/nullptr.hpp>

namespace wjson{

template<typename T, T metric>
class serializerT< size_value<T, metric> >
{
  typedef T value_type;
public:

  template<typename P>
  P operator()( const T& v, P beg)
  {
    int64_t bytes = static_cast<int64_t>(v) * static_cast<int64_t>(metric);

    bool writed =  false;

    if ( metric % 10 == 0 || metric==1)
    {
      writed = write_if_<1000, 4>(beg, bytes, "TB")
            || write_if_<1024, 4>(beg, bytes, "TiB")
            || write_if_<1000, 3>(beg, bytes, "GB")
            || write_if_<1024, 3>(beg, bytes, "GiB")
            || write_if_<1000, 2>(beg, bytes, "MB")
            || write_if_<1024, 2>(beg, bytes, "MiB")
            || write_if_<1000, 1>(beg, bytes, "KB")
            || write_if_<1024, 1>(beg, bytes, "KiB");
    }
    else
    {
      writed = write_if_<1024, 4>(beg, bytes, "TiB")
            || write_if_<1000, 4>(beg, bytes, "TB")
            || write_if_<1024, 3>(beg, bytes, "GiB")
            || write_if_<1000, 3>(beg, bytes, "GB")
            || write_if_<1024, 2>(beg, bytes, "MiB")
            || write_if_<1000, 2>(beg, bytes, "MB")
            || write_if_<1024, 1>(beg, bytes, "KiB")
            || write_if_<1000, 1>(beg, bytes, "KB");
    }

    if ( !writed )
      return typename value<T>::serializer()(v, beg);

    return beg;
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {

    if (beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    v = T();

    if ( parser::is_null(beg, end) )
    {
      v = T();
      return parser::parse_null(beg, end, e);
    }

    if ( parser::is_number(beg, end) )
      return typename value<T>::serializer()(v, beg, end, e);
    else if ( !parser::is_string(beg, end))
      return create_error<error_code::InvalidString>( e, end );

    // Десериализуем строку
    char buf[32]={0};
    beg = typename value<char[32]>::serializer()(buf, beg, end, e);
    if ( (e!=fas_nullptr && *e) ) return beg;

    const char* buf_beg = buf;
    const char* buf_end = buf + std::strlen(buf);

    int64_t val = 0;
    buf_beg = typename value<int64_t>::serializer()(val, buf_beg, buf_end, e);
    if ( (e!=fas_nullptr && *e) ) return beg;

    // Нет суффиксов и все корректно, значит в строке просто число
    if ( buf_beg==buf_end)
    {
      v = static_cast<T>(val);
      return beg;
    }

    // Первый символ T, G, M или K
    // Второй символ i (опционально, для степени 2)
    char ch1=*(buf_beg++);
    char ch2=0;
    if (buf_beg!=buf_end)
      ch2=*(buf_beg++);
    int64_t mult = 1;
    if ( ch1=='T' )
      mult = ch2=='i' ? 1024ul*1024*1024*1024 : 1000000000000ul;
    else if ( ch1=='G' )
      mult = ch2=='i' ? 1024*1024*1024 : 1000000000;
    else if ( ch1=='M')
      mult = ch2=='i' ? 1024*1024 : 1000000;
    else if (ch1=='K')
      mult = ch2=='i' ? 1024 : 1000;
    else if (ch1=='B')
      mult = 1;
    else
      return create_error<error_code::InvalidNumber>( e, end );

    v = static_cast<T>( (val * mult) / static_cast<int64_t>(metric) );
    return beg;
  }

private:

  template<value_type Base, value_type Exp, typename P>
  bool write_if_(P& beg, int64_t v, const char* ch)
  {
    return write_if_1_<Base, Exp>(beg, v, ch, fas::bool_< (sizeof(value_type)/2 >= Exp) >() );
  }


  template<value_type Base, value_type Exp, typename P>
  bool write_if_1_(P& beg, int64_t v, const char* ch, fas::true_)
  {
    typedef typename fas::pow< fas::integral_c<value_type, Base>,  fas::integral_c<value_type, Exp> > pow;
    bool ready = v!=0
        ? v % pow::value == 0
        : metric == pow::value;
    if ( ready )
    {
      int64_t wv = v/pow::value;
      *(beg++) = '"';
      typename wjson::value<int64_t>::serializer()(wv, beg);
      for (;*ch!='\0'; ++ch)
        *(beg++)=*ch;
      *(beg++) = '"';
    }
    return ready;
  }

  template<value_type Base, value_type Exp, typename P>
  bool write_if_1_(P& , int64_t& , const char* , fas::false_)
  {
    return false;
  }

};

}
