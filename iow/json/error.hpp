//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <cstddef>
#include <iterator>

namespace iow{ namespace json{

struct error_code
{
  typedef enum{
    ValidJSON = 0,
    InvalidJSON = 1,
    UnexpectedEndFragment = 2,
    ExpectedOf = 3,
    InvalidNull = 4,
    InvalidNumber = 5,
    InvalidBool = 6,
    InvalidString = 7,
    InvalidMember = 8,
    InvalidEnum = 9
  } type;
};


class json_error
{
public:
      
  json_error()
    : _code(error_code::ValidJSON)
    , _what(0)
    , _tail_of(0) 
  {}
  
  json_error(error_code::type code,  size_t tail_of = 0 )
    : _code(code)
    , _what(0)
    , _tail_of(tail_of) 
  {}

  json_error(error_code::type code,  const char *sym, size_t tail_of = 0 )
    : _code(code)
    , _what(sym)
    , _tail_of(tail_of) 
  {}
  
  error_code::type code() const
  {
    return _code;
  }

  const char* what() const 
  {
    return _what;
  }

  size_t tail_of() const 
  {
    return _tail_of; 
  }

  operator bool () const 
  {
    return _code!=error_code::ValidJSON; 
  }

  void reset() 
  { 
    _code = error_code::ValidJSON;
    _what=0;
    _tail_of=0;
  }

  template<typename P>
  size_t where( P beg, P end) const
  {
    if ( _what==0 )
      return 0;

    if (std::distance(beg, end) < _tail_of )
      return 0;

    return std::distance(beg, end) - _tail_of;
  }

  
private:

  template<typename L, typename R>
  const char* what_( fas::type_list<L, R> )
  {
    if ( _code == static_cast<int>(L::value) )
      return L();
    return this->what_(R());
  }

  const char* what_( fas::empty_list )
  {
    return "";
  }
private:
  error_code::type _code;
  const char* _what;
  std::ptrdiff_t _tail_of;
};

template<error_code::type code, typename Itr>
inline Itr create_error(json_error* e, Itr end, size_t tail_of = 0)
{
  if ( e != NULL && !*e )
    *e = json_error(code, tail_of);
  return end;
}

template<error_code::type code, typename Itr >
inline Itr create_error(json_error* e, Itr end, const char* msg, size_t tail_of = 0)
{
  if ( e != NULL && !*e)
    *e = json_error(code, msg, tail_of);
  return end;
}

}}
