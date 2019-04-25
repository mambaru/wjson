#include <wjson/json.hpp>
#include <iostream>
#include <cstring>

int main()
{
  const char* english = "\"hello world!\"";
  const char* russian = "\"\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442\\u0020\\u043C\\u0438\\u0440\\u0021\"";
  const char* chinese = "\"\\u4E16\\u754C\\u4F60\\u597D!\"";
  
  typedef char str_t[128];
  typedef ::wjson::value< std::string, 128  >::serializer sser_t;
  typedef ::wjson::value< std::vector<char> >::serializer vser_t;
  typedef ::wjson::value< str_t >::serializer             aser_t;

  std::string sstr;
  std::vector<char> vstr;
  str_t astr={'\0'};
  
  // Десериализация
  sser_t()( sstr, english, english + std::strlen(english), NULL);
  vser_t()( vstr, russian, russian + std::strlen(russian), NULL);
  aser_t()( astr, chinese, chinese + std::strlen(chinese), NULL);

  // Результат
  std::cout << "English: " << sstr << "\tfrom JSON: " << english << std::endl;
  std::cout << "Russian: " << std::string(vstr.begin(), vstr.end() ) << "\tfrom JSON: " << russian << std::endl;
  std::cout << "Chinese: " << astr << "\tfrom JSON: " << chinese << std::endl;

  // Сериализация english в stdout
  std::cout << std::endl << "English JSON: ";
  sser_t()( sstr, std::ostream_iterator<char>( std::cout) );
  std::cout << "\tfrom: " << sstr;
  
  // Сериализация russian в stdout
  std::cout << std::endl << "Russian JSON: ";
  vser_t()( vstr, std::ostream_iterator<char>( std::cout) );
  std::cout << "\tfrom: " << std::string(vstr.begin(), vstr.end() );
  
  // Сериализация chinese в stdout
  std::cout << std::endl << "Chinese JSON: ";
  aser_t()( astr, std::ostream_iterator<char>( std::cout) );
  std::cout << "\tfrom: " << astr;
  std::cout << std::endl;
}
