#pragma once 

#include <string>
#include <sstream>
#include <iow/json/name.hpp>
#include <iow/json/json.hpp>

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

  template<typename E, typename Itr>
  static inline Itr create(json_error* e, Itr end, size_t tail_of = 0)
  {
    if ( e != nullptr && !*e )
      *e = E(tail_of);
    return end;
  }

  template<typename E, typename Itr >
  static inline Itr create(json_error* e, Itr end, const char* msg, size_t tail_of = 0)
  {
    if ( e != nullptr && !*e)
      *e = E(msg, tail_of);
    return end;
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
  error_code::type _code = error_code::ValidJSON;
  const char* _what;
  std::ptrdiff_t _tail_of;
};

class invalid_json: public json_error
{ 
public:
  invalid_json(size_t tail_of = 0)
    : json_error(error_code::InvalidJSON, tail_of)  {}
};

class invalid_json_null: public json_error
{ 
public:
  invalid_json_null(size_t tail_of = 0)
    : json_error(error_code::InvalidNull, tail_of)  {}
};


class invalid_json_number: public json_error
{ 
public:
  invalid_json_number(size_t tail_of = 0)
    : json_error(error_code::InvalidNumber, tail_of)  {}
};

class invalid_json_bool: public json_error
{ 
public:
  invalid_json_bool(size_t tail_of = 0)
    : json_error(error_code::InvalidBool, tail_of)  {}
};

class invalid_json_string: public json_error
{ 
public:
  invalid_json_string(size_t tail_of = 0)
    : json_error(error_code::InvalidString, tail_of)  {}
};

class invalid_json_member: public json_error
{ 
public:
  invalid_json_member(size_t tail_of = 0)
    : json_error(error_code::InvalidMember, tail_of)  {}
};

class unexpected_end_fragment
  : public json_error
{
public:
  unexpected_end_fragment(size_t tail_of = 0)
    : json_error( error_code::UnexpectedEndFragment, tail_of) {}
  unexpected_end_fragment(const char* str, size_t tail_of = 0)
   : json_error( error_code::UnexpectedEndFragment, str, tail_of ) {}
};

class expected_of
  : public json_error
{
public:
  expected_of(const char* sym, size_t tail_of = 0)
    : json_error( error_code::ExpectedOf, sym, tail_of) {}
};

}}
