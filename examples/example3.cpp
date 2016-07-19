#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cstring>

int main()
{
  const char* hello1 = "hello world!";
  const char* hello2 = "\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442\\u0020\\u043C\\u0438\\u0440\\u0021";
  const char* hello3 = "\\u4E16\\u754C\\u4F60\\u597D!";
  
  typedef char str_t[128];
  typedef ::wjson::value< std::string, 128  >::serializer strser_t;
  typedef ::wjson::value< std::vector<char> >::serializer vctser_t;
  typedef ::wjson::value< str_t >::serializer             arrser_t;

  std::string str = hello1;
  std::vector<char> vctstr(hello2, hello2 + std::strlen(hello2) );
  str_t arrstr={'\0'};
  std::strcpy( arrstr, hello3 );
  
  std::string  json, json1, json2, json3;
  strser_t()( str, std::back_inserter(json1));
  vctser_t()( vctstr, std::back_inserter(json2));
  arrser_t()( arrstr, std::back_inserter(json3));
  
  std::cout << "English JSON: " << json1 << " from: " << hello1 << std::endl;
  std::cout << "Russian JSON: " << json2 << " from: " << hello2 << std::endl;
  std::cout << "Chinese JSON: " << json3 << " from: " << hello3 << std::endl;

  json1="\""; json1+=hello1; json1+="\"";
  json2="\""; json2+=hello2; json2+="\"";
  json3="\""; json3+=hello3; json3+="\"";

  std::cout << "---> " << hello3 << std::endl;
  
  ::wjson::json_error e;
  strser_t()( str, json1.begin(), json1.end(), &e);
  //if ( e ) std::cout << json1 << std::endl;
  vctser_t()( vctstr, json2.begin(), json2.end(), 0);
  arrser_t()( arrstr, json3.begin(), json3.end(), 0);

  std::cout << "English :" << str << " from JSON: " << json1 << std::endl;
  std::cout << "Russian :" << std::string(vctstr.begin(), vctstr.end() ) << " from JSON: " << json2 << std::endl;
  std::cout << "Chinese :" << arrstr << " from JSON: " << json3 << std::endl;

  return 0;
}