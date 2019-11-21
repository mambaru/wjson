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

struct bar: foo
{
  std::vector<int> data;
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

JSON_NAME(data)
typedef ::wjson::array< std::vector< ::wjson::value<int> > > vint_json;
typedef wjson::object<
  bar,
  wjson::member_list<
    wjson::base<foo_json>,
    wjson::member<n_data, bar, std::vector<int>,  &bar::data, vint_json>
  >
> bar_json;

typedef wjson::object<
  bar,
  wjson::member_list<
    wjson::member<n_flag, foo,bool,  &foo::flag>,
    wjson::member<n_value, foo,int,  &foo::value>,
    wjson::member<n_string, foo,std::string,  &foo::string>,
    wjson::member<n_data, bar, std::vector<int>,  &bar::data, vint_json>
  >
> bar2_json;

int main()
{
  std::string json="{\"flag\":false,\"value\":0,\"string\":\"Привет Мир\",\"data\":[1,2]";
  bar b;
  bar_json::serializer()( b, json.begin(), json.end(), fas_nullptr );
  b.flag = true;
  b.data.push_back(3);
  std::cout << json << std::endl;
  bar2_json::serializer()(b, std::ostream_iterator<char>(std::cout) );
  std::cout << std::endl;

  // {"flag":false,"value":0,"string":"Привет Мир","data":[1,2]
  // {"flag":true,"value":0,"string":"Привет Мир","data":[1,2,3]}
}

