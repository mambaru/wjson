#include <string>
#include <sstream>

namespace iow{ namespace json{

class json_error
{
public:
  json_error(): _what1(0), _what2(0), _tail_of(0) {}
  json_error(const char *msg, size_t tail_of = 0 ): _what1(msg), _what2(0), _tail_of(tail_of) {}
  json_error(const char *msg, const char *sym, size_t tail_of = 0 ): _what1(msg), _what2(sym), _tail_of(tail_of) {}
  size_t tail_of() const { return _tail_of; }
  const char* what() const 
  {
    if ( _what1==0 )
      return "OK";
    return _what1;
    
  }

  operator bool () const {return _what1!=0; }
  void reset() { _what1=0; _what2=0; _tail_of=0;}

  std::string message( ) const
  {
    std::stringstream ss;
    ss << _what1;
    if ( _what2 != 0 )
      ss << " '" << _what2 << "'.";
    return ss.str();
  }

  template<typename P>
  size_t where( P beg, P end) const
  {
    if ( _what1==0 )
      return 0;

    if (std::distance(beg, end) < _tail_of )
      return 0;

    return std::distance(beg, end) - _tail_of;
  }

  template<typename P>
  std::string trace( P beg, P end ) const
  {
    if ( this->what() == 0 )
      return 0;

    std::stringstream ss;
    ss  << std::string(beg, end - _tail_of ) << (_tail_of ? ">>>" : "<<<") << std::string(end - _tail_of, end);
    return ss.str();
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
  const char* _what1;
  const char* _what2;
  std::ptrdiff_t _tail_of;
};

/*
class serialize_error: public json_error
{ 
public:
  serialize_error(size_t tail_of = 0)
    : json_error("Serialize error", tail_of) {}
};
*/
/*
class deserialize_error: public json_error
{ 
public:
  deserialize_error(size_t tail_of = 0)
    : json_error("deserialize error", tail_of)  {}
};
*/
/*
class not_supported: public json_error
{ 
public:
  not_supported(size_t tail_of = 0)
    : json_error("not supported", tail_of)  {}
};
*/

class invalid_json: public json_error
{ 
public:
  invalid_json(size_t tail_of = 0)
    : json_error("Invalid json", tail_of)  {}
};

class invalid_json_null: public json_error
{ 
public:
  invalid_json_null(size_t tail_of = 0)
    : json_error("Invalid json null", tail_of)  {}
};


class invalid_json_number: public json_error
{ 
public:
  invalid_json_number(size_t tail_of = 0)
    : json_error("Invalid json number", tail_of)  {}
};

class invalid_json_bool: public json_error
{ 
public:
  invalid_json_bool(size_t tail_of = 0)
    : json_error("Invalid json bool", tail_of)  {}
};

class invalid_json_string: public json_error
{ 
public:
  invalid_json_string(size_t tail_of = 0)
    : json_error("Invalid json string", tail_of)  {}
};

class invalid_json_member: public json_error
{ 
public:
  invalid_json_member(size_t tail_of = 0)
    : json_error("Invalid json member", tail_of)  {}
};

/*
class invalid_json_object: public json_error
{ 
public:
  invalid_json_object(size_t tail_of = 0)
    : json_error("Invalid json object", tail_of)  {}
};
*/

/*
class invalid_json_array: public json_error
{ 
public:
  invalid_json_array(size_t tail_of = 0)
    : json_error("Invalid json array", tail_of)  {}
};
*/

/*
class invalid_conversion
  : public json_error
{
public:
  invalid_conversion(size_t tail_of = 0)
    : json_error( "invalid conversion", tail_of) {}
  invalid_conversion( const char* from, const char* to, size_t tail_of = 0 )
    : json_error( std::string("invalid conversion from '") + from + std::string("' to '") + to, tail_of ) {}
};
*/

class unexpected_end_fragment
  : public json_error
{
public:
  unexpected_end_fragment(size_t tail_of = 0)
    : json_error( "Unexpected end of ragment", tail_of) {}
    
  unexpected_end_fragment(const char* str, size_t tail_of = 0)
   : json_error( "Unexpected end of ragment", str, tail_of ) {}
   
};

class expected_of
  : public json_error
{
public:
  expected_of(const char* sym, size_t tail_of = 0)
    : json_error( "Expected of", sym, tail_of) {}
};

}}
