//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/name.hpp>
#include <wjson/predef.hpp>
#include <wjson/error.hpp>
#include <string>
#include <sstream>

namespace wjson{

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
  JSON_NAME2(InvalidEnum, "Invalid Enum")
  JSON_NAME2(InvalidRequest, "Invalid JSON-RPC Request")
  
  typedef ::wjson::enumerator<
    int, 
    ::wjson::member_list<
      enum_value<ValidJSON, int, static_cast<int>(error_code::ValidJSON)>,
      enum_value<InvalidJSON, int, static_cast<int>(error_code::InvalidJSON)>,
      enum_value<UnexpectedEndFragment, int, static_cast<int>(error_code::UnexpectedEndFragment)>,
      enum_value<ExpectedOf, int, static_cast<int>(error_code::ExpectedOf)>,
      enum_value<InvalidNull, int, static_cast<int>(error_code::InvalidNull)>,
      enum_value<InvalidNumber, int, static_cast<int>(error_code::InvalidNumber)>,
      enum_value<InvalidBool, int, static_cast<int>(error_code::InvalidBool)>,
      enum_value<InvalidString, int, static_cast<int>(error_code::InvalidString)>,
      enum_value<InvalidMemberr, int, static_cast<int>(error_code::InvalidMember)>,
      enum_value<InvalidEnum, int, static_cast<int>(error_code::InvalidEnum)>,
      enum_value<InvalidRequest, int, static_cast<int>(error_code::InvalidRequest)>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

class strerror
{
public:
  static const char* what(const json_error& e)  
  {
    return strerror::what_( e.code(), error_code_json::member_list() );
  }

  static std::string message( const json_error& e )
  {
    std::stringstream ss;
    ss << strerror::what(e);
    if ( e.expected_of() != 0 )
      ss << " '" << e.expected_of() << "'";
    return ss.str();
  }

  template<typename P>
  static std::ptrdiff_t where( const json_error& e, P beg, P end)
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

  template<typename P>
  static std::string message_trace( const json_error& e, P beg, P end ) 
  {
    std::stringstream ss;
    ss << strerror::what(e);
    if ( e.expected_of() != 0 )
      ss << " '" << e.expected_of() << "'";
    if ( e )
    {
      ss  << ": "
          << std::string(beg, end - e.tail_of() ) 
          << ( e.tail_of() ? ">>>" : "<<<") 
          << std::string(end - e.tail_of(), end);
    }
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

}
