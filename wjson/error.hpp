//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <cstddef>
#include <iterator>

namespace wjson{

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
    , _expected_of(0)
    , _tail_of(0) 
  {}
  
  json_error(error_code::type code_,  std::ptrdiff_t tail_of_ = 0 )
    : _code(code_)
    , _expected_of(0)
    , _tail_of(tail_of_) 
  {}

  json_error(error_code::type code_,  const char *sym, std::ptrdiff_t tail_of_ = 0 )
    : _code(code_)
    , _expected_of(sym)
    , _tail_of(tail_of_) 
  {}

  error_code::type type() const
  {
    return _code;
  }

  int code() const
  {
    return static_cast<int>(_code);
  }

  const char* expected_of() const 
  {
    return _expected_of;
  }

  std::ptrdiff_t tail_of() const 
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
    _expected_of=0;
    _tail_of=0;
  }
  
private:
  error_code::type _code;
  const char* _expected_of;
  std::ptrdiff_t _tail_of;
};

template<error_code::type code, typename Itr>
inline Itr create_error(json_error* e, Itr end, std::ptrdiff_t tail_of = 0)
{
  if ( e != NULL && !*e )
    *e = json_error(code, tail_of);
  return end;
}

template<error_code::type code, typename Itr >
inline Itr create_error(json_error* e, Itr end, const char* msg, std::ptrdiff_t tail_of = 0)
{
  if ( e != NULL && !*e)
    *e = json_error(code, msg, tail_of);
  return end;
}

}
