#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

struct foo
{
  bool flag;
  int value;
  std::string string;
  foo(): flag(false), value(0) {}
};

JSON_NAME(flag)
JSON_NAME(value)
JSON_NAME(string)

typedef wjson::object<
  foo,
  wjson::member_list<
    wjson::member<n_flag, foo,bool,  &foo::flag>,
    wjson::member<n_value, foo,int,  &foo::value>,
    wjson::member<n_string, foo,std::string,  &foo::string>
  >
> foo_json;

int main()
{
  std::string json="{\"flag\":false,\"value\":0,\"string\":\"Привет Мир\"}";
  foo f;
  foo_json::serializer()( f, json.begin(), json.end(), NULL );

  f.flag = true;
  f.string = "Пока Мир";
  f.value = 1;
  std::cout << json << std::endl;
  foo_json::serializer()( f, std::ostream_iterator<char>(std::cout) );

  // Out:
  // {"flag":false,"value":0,"string":"Привет Мир"}
  // {"flag":true,"value":1,"string":"Пока Мир"}
  return 0;
}
