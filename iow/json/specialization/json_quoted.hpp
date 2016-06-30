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
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( ReqQ && *beg == '"')
      ++beg;
    else
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );
    
    beg = serializer()(t, beg, end, e);

    if ( ReqQ && beg == end)
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( ReqQ && *beg == '"')
      ++beg;
    else
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );

    return beg;
  }
};

template<typename J, typename SJ, bool SerQ, bool ReqQ, int R>
class serializerQ {
public:
  typedef typename J::serializer serializer_t;
  typedef typename J::target target;
  
  /** serialization:
   * end - back inserter iterator
   */
  template<typename P>
  P operator()( const target& t, P end)
  {
    return this->serialize(t, end, fas::bool_<SerQ>() );
  }

  /** deserialization:
   * beg, end - forward iterator
   */
  template<typename P>
  P operator()( target& t, P beg, P end, json_error* e)
  {
    return this->deserialize(t, beg, end, e, fas::bool_<ReqQ>() );
  }

private:

  template<typename P>
  P serialize( const target& t, P end, fas::true_)
  {
    typename SJ::target buf;
    buf.reserve( (R >= 0) ? R : 64 );
    serializer_t()( t, std::back_inserter(buf) );
    return typename SJ::serializer()( buf, end );
  }

  template<typename P>
  P serialize( const target& t, P end, fas::false_)
  {
    return serializer_t()( t, end );
  }

  template<typename P>
  P deserialize( target& t, P beg, P end, json_error* e, fas::true_)
  {
    typename SJ::target buf;
    buf.reserve( (R >= 0) ? R : 64 );
    beg = typename SJ::serializer()( buf, beg, end, e );
    if ( e && *e ) return end;

    json_error e2;
    serializer_t()( t, buf.begin(), buf.end(), &e2 );
    if (e && e2)
    {
      *e = json_error( e2.code(), std::distance(beg, end) + e2.tail_of() + 1 );
      return end;
    }
    return beg;
  }

  template<typename P>
  P deserialize( target& t, P beg, P end, json_error* e, fas::false_)
  {
    if ( parser::is_string(beg, end) )
      return this->deserialize(t, beg, end, e, fas::true_() );
    return serializer_t()( t, beg, end, e );
  }

};

}}
