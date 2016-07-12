#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cstring>

int main()
{
  typedef ::wjson::value<long double>::serializer serializer_t;
  typedef ::wjson::value<long double, 4 >::serializer serializer4_t;
  long double value = 0.00000001234501;
  std::cout << value << std::endl;

  std::string json;
  serializer_t()(value, std::back_inserter(json)); 
  std::cout << json << std::endl;
  value=-1;
  serializer_t()(value, json.begin(), json.end(), 0 ); 
  std::cout << value << std::endl;
  value=-1;
  serializer4_t()(value, json.begin(), json.end(), 0 ); 
  std::cout << value << std::endl;

  std::cout << "---" << std::endl;
  value=12345.12345;
  json.clear();
  serializer4_t()(value, std::back_inserter(json)); 
  std::cout << json << std::endl;
  value=-1;
  serializer4_t()(value, json.begin(), json.end(), 0 ); 
  std::cout << value << std::endl;
  value=-1;
  serializer_t()(value, json.begin(), json.end(), 0 ); 
  std::cout << value << std::endl;
  return 0;
}