#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cstring>

int main()
{
  typedef ::wjson::value<double>::serializer serializer_t;
  double value = 12345;

  std::string json;
  serializer_t()(value, std::back_inserter(json)); 
  std::cout << json << std::endl;
  json[ json.find('+') ]='*';
  std::cout << json << std::endl;

  value = 0.0;
  wjson::json_error e;
  serializer_t()(value, json.begin(), json.end(), &e ); 

  if ( e )
  {
    std::cout << "Error code: " << e.code() << std::endl;
    std::cout << "Error position: " << ::wjson::strerror::where( e, json.begin(), json.end() ) << std::endl;
    std::cout << "Error tail of: " << e.tail_of() << std::endl;
    //std::cout << "Error what: " << e.what() << std::endl;
    std::cout << "Error message: " << ::wjson::strerror::message(e) << std::endl;
    std::cout << "Error trace: " << ::wjson::strerror::trace(e, json.begin(), json.end()) << std::endl;
    std::cout << "Error message & trace: " << ::wjson::strerror::message_trace(e, json.begin(), json.end()) << std::endl;
  }
  return 0;
}
