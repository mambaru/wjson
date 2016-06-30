#include <string>
#include <sstream>
#include <iow/json/name.hpp>
#include <iow/json/json.hpp>
#include <iow/json/error.hpp>

namespace iow{ namespace json{

struct error_code_json
{
  JSON_NAME2(ValidJSON, "Valid JSON")
  JSON_NAME2(InvalidJSON, "Invalid JSON")
  JSON_NAME2(UnexpectedEndFragment, "Unexpected End of Fragment")
  JSON_NAME2(ExpectedOf, "Expected Of")
  JSON_NAME2(InvalidNull, "Invalid Null")
  JSON_NAME2(InvalidNumber, "Invalid Number")
  JSON_NAME2(InvalidBool, "Invalid Bool")
  JSON_NAME2(InvalidString, "Invalid String")
  JSON_NAME2(InvalidMemberr, "Invalid Member")
  
  typedef ::iow::json::enumerator<
    int, 
    member_list<
      enum_value<ValidJSON, int, static_cast<int>(error_code::ValidJSON)>,
      enum_value<InvalidJSON, int, static_cast<int>(error_code::InvalidJSON)>,
      enum_value<UnexpectedEndFragment, int, static_cast<int>(error_code::UnexpectedEndFragment)>,
      enum_value<ExpectedOf, int, static_cast<int>(error_code::ExpectedOf)>,
      enum_value<InvalidNull, int, static_cast<int>(error_code::InvalidNull)>,
      enum_value<InvalidNumber, int, static_cast<int>(error_code::InvalidNumber)>,
      enum_value<InvalidBool, int, static_cast<int>(error_code::InvalidBool)>,
      enum_value<InvalidString, int, static_cast<int>(error_code::InvalidString)>,
      enum_value<InvalidMemberr, int, static_cast<int>(error_code::InvalidMember)>
    >
  > type;
};

class strerror
{
public:
  static const char* what(const json_error& e)  
  {
    return strerror::what_( e.code(), error_code_json::type::member_list() );
  }

  static std::string message( const json_error& e )
  {
    std::stringstream ss;
    ss << strerror::what(e);
    if ( e.what() != 0 )
      ss << " '" << e.what() << "'";
    return ss.str();
  }

  template<typename P>
  static size_t where( const json_error& e, P beg, P end)
  {
    if ( !e ) return 0;

    if (std::distance(beg, end) < e.tail_of() )
      return 0;

    return std::distance(beg, end) - e.tail_of();
  }

  template<typename P>
  static std::string trace( const json_error& e, P beg, P end ) 
  {
    if ( !e )
      return std::string(beg, end);

    std::stringstream ss;
    ss  << std::string(beg, end - e.tail_of() ) << ( e.tail_of() ? ">>>" : "<<<") << std::string(end - e.tail_of(), end);
    return ss.str();
  }
    
private:

  template<typename L, typename R>
  static const char* what_( int code, fas::type_list<L, R> )
  {
    if ( code == static_cast<int>(L::value) )
      return L()();
    return strerror::what_(code, R());
  }

  static const char* what_( int , fas::empty_list )
  {
    return "";
  }
};


}}
