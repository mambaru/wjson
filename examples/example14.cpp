#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

enum class error_code
{
  ValidJSON = 0,
  InvalidJSON = 1,
  ParseError = 2
};

struct error
{
  int code;
};

struct code_json
{
  JSON_NAME2(ValidJSON, "Valid JSON.")
  JSON_NAME2(InvalidJSON, "Invalid JSON.")
  JSON_NAME2(ParseError, "Parse Error.")
 
  typedef ::wjson::enumerator<
    int,
    ::wjson::member_list<
      ::wjson::enum_value< ValidJSON,   int, static_cast<int>(error_code::ValidJSON)>,
      ::wjson::enum_value< InvalidJSON, int, static_cast<int>(error_code::InvalidJSON)>,
      ::wjson::enum_value< ParseError,  int, static_cast<int>(error_code::ParseError)>
    > 
  > type;

  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list; 
};

struct error_json
{
  JSON_NAME(code)
  JSON_NAME(message)
 
  typedef ::wjson::object<
    error,
    ::wjson::member_list<
      ::wjson::member< n_code,    error, int, &error::code>,
      ::wjson::member< n_message, error, int, &error::code, code_json>
    > 
  > type;

  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list; 
};

int main()
{
  error e;
  e.code = static_cast<int>(error_code::InvalidJSON);
  error_json::serializer()(e, std::ostream_iterator<char>(std::cout) );
  std::cout << std::endl;
  // {"code":1,"message":"Invalid JSON."}
}
