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
 * SerQ = сериализовывать с кавычками
 * ReqQ = только строки
 */
template<typename J, bool SerQ, bool ReqQ>
class serializerRQ {
public:
  typedef J serializer;
  typedef typename J::target target;
  
  /** serialization:
   * end - back inserter iterator
   */
  template<typename P>
  P operator()( const target& t, P end)
  {
    if ( SerQ ) *(end++)='"';
    end = serializer()(t, end);
    if ( SerQ ) *(end++)='"';
    return end;
  }

  /** deserialization:
   * beg, end - forward iterator
   */
  template<typename P>
  P operator()( target& t, P beg, P end, json_error* e)
  {
    if (beg == end)
      return json_error::create<unexpected_end_fragment>(e, end);

    if ( ReqQ && *beg == '"')
      ++beg;
    else
      return json_error::create<expected_of>(e, end, "\"", std::distance(beg, end) );
    
    beg = serializer()(t, beg, end, e);

    if ( ReqQ && beg == end)
      return json_error::create<unexpected_end_fragment>(e, end);

    if ( ReqQ && *beg == '"')
      ++beg;
    else
      return json_error::create<expected_of>(e, end, "\"", std::distance(beg, end) );

    return beg;
  }
};

}}
