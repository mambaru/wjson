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

struct boo: foo
{
  foo f;
  std::vector<foo> fl;
}

struct foo_json
{
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
  > meta;

  typedef meta::target target;
  typedef meta::member_list member_list;
  typedef meta::serializer serializer;

  static const char* comment(n_flag) { return "Это флаг"; }
};

int main()
{
  return 0;
}
