#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

int main()
{
  typedef wjson::array< std::vector< wjson::value<int>  > >::serializer serializer_t;
  std::vector< int > value;
  std::string json = "[1,2,3}5,6]";
  wjson::json_error e;
  serializer_t()(value, json.begin(), json.end(), &e ); 
  if ( e )
  {
    std::cout << "Error code: " << e.code() << std::endl;
    std::cout << "Error tail of: " << e.tail_of() << std::endl;
    if ( e.type() == wjson::error_code::ExpectedOf )
      std::cout << "Error expected_of: " << e.expected_of() << std::endl;

    std::cout << "Error position: " << wjson::strerror::where(e, json.begin(), json.end() ) << std::endl;
    std::cout << "Error message: " << wjson::strerror::message(e) << std::endl;
    std::cout << "Error trace: " << wjson::strerror::trace(e, json.begin(), json.end()) << std::endl;
    std::cout << "Error message & trace: " << wjson::strerror::message_trace(e, json.begin(), json.end()) << std::endl;
  }
  /*
    Error code: 3
    Error tail of: 5
    Error expected_of: ,
    Error position: 6
    Error message: Expected Of ','
    Error trace: [1,2,3>>>}5,6]
    Error message & trace: Expected Of ',': [1,2,3>>>}5,6]
  */
}
