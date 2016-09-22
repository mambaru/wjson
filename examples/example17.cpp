#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

template<typename P>
inline P show_parse( P beg, P end, wjson::json_error* e )
{
  using namespace wjson;
  // игнорируем пробельные символы 
  beg = parser::parse_space(beg, end, e);
  // проверяем, что это масссив
  if ( !parser::is_array(beg, end) )
    return create_error<error_code::ExpectedOf>( e, end, "[", std::distance(beg, end) );
  // до тех пор пока не достигнем конца массива или буфера
  for ( ++beg; beg!=end && *beg!=']'; )
  {
    // игнорируем пробельные символы после [ или ,
    beg = parser::parse_space(beg, end, e);
    // пустой массив или конец
    if ( *beg==']' ) break;

    if ( wjson::parser::is_string(beg,end) )
    {
      // если элемент являеться строкой, то десериализуем и выводим 
      std::string val;
      beg = ::wjson::value<std::string>::serializer()(val, beg, end, e);
      if (*e) return end;
      std::cout << "String " << val << std::endl;
    }
    else 
    {
      // все остальное просто парсим
      beg = parser::parse_value(beg, end, e);
    }
    // игнорируем пробельные символы после текущего элемента
    beg = parser::parse_space(beg, end, e);
    // проверяем, что следующий символ , или ]
    if (beg == end || ( *beg!=',' && *beg!=']' ) ) 
      return create_error<error_code::ExpectedOf>( e, end, "]", std::distance(beg, end) );
    // если , то далее, для ] сработает условие в начале
    if ( *beg==',' ) ++beg;
  }
  // при успешном парсинге, текщим символом будет ] 
  if (beg == end || *beg!=']') 
    return create_error<error_code::ExpectedOf>( e, end, "]", std::distance(beg, end) );
  // переходим за ]
  ++beg;
  // Done
  return beg;
}

int main()
{
  std::string json = "/*еще один пример*/ [ 42, null, /*что это?*/ \"абракадабра\", [1,2], {\"x\":[3, 4]}, 7, true ]";
  wjson::json_error e;
  show_parse( json.begin(), json.end(), &e);
  if ( !e )
    std::cout << "Done" << std::endl;
  else
    std::cout << wjson::strerror::message_trace(e, json.begin(), json.end()) << std::endl;
}
