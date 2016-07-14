#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cstring>

int main()
{
  typedef ::wjson::array< std::vector< ::wjson::value<int>  > >::serializer serializer_t;
  std::vector< int > value;
  std::string json = "[1,2,3,'error',5,6]";
  ::wjson::json_error e;
  serializer_t()(value, json.begin(), json.end(), &e ); 
  if ( e )
  {
    std::cout << "Error code: " << e.code() << std::endl;
    std::cout << "Error tail of: " << e.tail_of() << std::endl;
    std::cout << "Error position: " << ::wjson::strerror::where(e, json.begin(), json.end() ) << std::endl;
    std::cout << "Error expected_of: " << e.expected_of() << std::endl;
    std::cout << "Error message: " << ::wjson::strerror::message(e) << std::endl;
    std::cout << "Error trace: " << ::wjson::strerror::trace(e, json.begin(), json.end()) << std::endl;
    std::cout << "Error message & trace: " << ::wjson::strerror::message_trace(e, json.begin(), json.end()) << std::endl;
  }
  return 0;
}