/*
 * json_quoted.hpp
 *
 *  Created on: Feb 24, 2014
 *      Author: raven
 */

namespace iow{ namespace json{

/**
 * сериализует с обязательным обрамлением кавычками
 * нужно для сериализации map{ int => value }
 */
template<typename T>
class serializerQuoted {
public:
  serializerT< value<T> > serializer;
  serializerQuoted(){}

  /** serialization:
   * end - back inserter iterator
   */
  template<typename P>
  P operator()( const T& t, P end)
  {
    *(end++)='"';
    end = serializer(t, end);
    *(end++)='"';
    return end;
  }

  /** deserialization:
   * beg, end - forward iterator
   */
  template<typename P>
  P operator()( T& t, P beg, P end)
  {
    if ( parser::is_null(beg, end) )
    {
      return parser::parse_null(beg, end);
    }

    if (beg == end)
    {
      throw unexpected_end_fragment();
    }

    if (*(beg++) != '"')
    {
      throw unexpected_end_fragment();
    }

    beg = serializer(t, beg, end);
    if (beg == end)
    {
      throw unexpected_end_fragment();
    }

    if (*(beg++)!='"')
    {
      throw unexpected_end_fragment();
    }
    return beg;
  }
};

template<typename T>
struct value_quoted {
  typedef T target;
  typedef serializerQuoted< T > serializer;
};

}}
