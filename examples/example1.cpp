#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cstring>

int main()
{
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

  std::cout << "--- tmp ---" << std::endl;

  /* Правильный формат. Вывод: true */
  bool flag = false;
  std::string json="true";
  ::wjson::value<bool>::serializer()(flag, json.begin(), json.end(), 0);
  std::cout << std::boolalpha;
  std::cout << flag << std::endl;

  /* Неправильный формат. Вывод: false */
  flag = false;
  json="1";
  ::wjson::value<bool>::serializer()(flag, json.begin(), json.end(), 0);
  std::cout << flag << std::endl;

  /* Неправильный формат с обработкой ошибки. Вывод: false: Invalid Bool*/
  flag = false;
  json="1";
  ::wjson::json_error e;
  ::wjson::value<bool>::serializer()(flag, json.begin(), json.end(), &e);
  std::cout << flag << ": ";
  std::cout << ::wjson::strerror::message(e) << std::endl;
  }

  {
  typedef ::wjson::value<short>::serializer serializer_t;
  short value = 0;
  std::string json="12356789XYZ";
  ::wjson::json_error e;
  std::string::iterator itr = serializer_t()(value, json.begin(), json.end(), &e );
  std::cout << value << ": ";
  std::cout << ::wjson::strerror::message(e) << std::endl;
  std::cout << "Tail: " << std::string( itr, json.end() ) << std::endl; 
  serializer_t()(value, std::ostreambuf_iterator<char>(std::cout));
  std::cout << std::endl; 
  }
/*
  flag = false;
  json="1";
  ::wjson::quoted< ::wjson::value<bool>, false, false >::serializer()(flag, json.begin(), json.end(), 0);
  std::cout << std::boolalpha;
  std::cout << flag << std::endl;
*/
  
  return 0;
}