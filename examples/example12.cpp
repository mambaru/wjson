#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

struct foo
{
  bool flag;
  int value;
  std::string string;
  foo(): flag(false), value(0), string("") {}
};

struct bar: foo
{
  std::shared_ptr<foo> pfoo;
  std::vector<foo> vfoo;
  bar(){}
};

struct foo_json
{
  JSON_NAME(flag)
  JSON_NAME(value)
  JSON_NAME(string)

  typedef wjson::object_array<
    foo,
    wjson::member_list<
      wjson::member<n_flag, foo,bool,  &foo::flag>,
      wjson::member<n_value, foo,int,  &foo::value>,
      wjson::member<n_string, foo,std::string,  &foo::string>
    >
  > type;
  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list;
};

struct bar_json
{
  typedef ::wjson::array< std::vector< foo_json > > vfoo_json;
  typedef ::wjson::pointer< std::shared_ptr<foo>, foo_json > pfoo_json;

  typedef wjson::object_array<
    bar,
    wjson::member_list<
      wjson::base<foo_json>,
      wjson::member_array<bar, std::vector<foo>,  &bar::vfoo, vfoo_json>,
      wjson::member_array<bar, std::shared_ptr<foo>,  &bar::pfoo, pfoo_json>
    >
  > type;
  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list;
};

int main()
{
  std::string json="[true,0,\"Привет Мир\",[],\"pfoo\":null]";
  bar b;
  bar_json::serializer()( b, json.begin(), json.end(), fas_nullptr );
  b.flag = true;
  b.vfoo.push_back( static_cast<const foo&>(b));
  b.pfoo = std::make_shared<foo>(static_cast<const foo&>(b));
  std::cout << json << std::endl;
  bar_json::serializer()(b, std::ostream_iterator<char>(std::cout) );
  std::cout << std::endl;

  // [true,0,"Привет Мир",[],"pfoo":null]
  // [true,0,"Привет Мир",[[true,0,"Привет Мир"]],[true,0,"Привет Мир"]]
}
