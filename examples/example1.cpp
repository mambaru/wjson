#include <wjson/json.hpp>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cstring>

int main()
{
  typedef ::wjson::value<int>::serializer serializer_t;
  int value = 12345;

  std::cout << "--- serializer ---" << std::endl;
  
  char bufjson[100];
  char* ptr = serializer_t()(value, bufjson); 
  *ptr = '\0';
  std::cout << bufjson << std::endl;

  std::string strjson;
  serializer_t()(value, std::back_inserter(strjson)); 
  std::cout << strjson << std::endl;

  std::stringstream ssjson;
  serializer_t()(value, std::ostreambuf_iterator<char>(ssjson)); 
  std::cout << ssjson.str() << std::endl;

  serializer_t()(value, std::ostreambuf_iterator<char>(std::cout)); 
  std::cout << std::endl;

  std::cout << "--- unserializer ---" << std::endl;
  value = 0;
  serializer_t()(value, bufjson, bufjson + strlen(bufjson), 0 );
  std::cout << value << std::endl;
  value = 0;
  serializer_t()(value, strjson.begin(), strjson.end(), 0 );
  std::cout << value << std::endl;
  
  return 0;
}