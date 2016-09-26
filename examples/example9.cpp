#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

struct foo
{
  bool flag = false;
  int value = 0;
  std::string string;
};

struct bar: foo
{
  std::shared_ptr<foo> pfoo;
  std::vector<foo> vfoo;
};

/*struct foo_json
{*/
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
  > foo_json;/*type;
  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list; 
};*/

struct bar_json
{
  JSON_NAME(pfoo)
  JSON_NAME(vfoo)
  typedef ::wjson::array< std::vector< foo_json > > vfoo_json;
  typedef ::wjson::pointer< std::shared_ptr<foo>, foo_json > pfoo_json;
  
  typedef wjson::object<
    bar,
    wjson::member_list<
      wjson::base<foo_json>,
      wjson::member<n_vfoo, bar, std::vector<foo>,  &bar::vfoo, vfoo_json>,
      wjson::member<n_pfoo, bar, std::shared_ptr<foo>,  &bar::pfoo, pfoo_json>
    >
  > type;
  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list; 
};

template<typename J>
struct deserealizer
{
  typedef typename J::deserializer type;
};

int main()
{
  // typedef deserealizer<bar_json::type>::type deser;

  std::string json="{\"flag\":true,\"value\":0,\"string\":\"Привет Мир\",\"vfoo\":[],\"pfoo\":null}";
  bar b;
  bar_json::serializer()( b, json.begin(), json.end(), nullptr );
  
  b.flag = true;
  b.vfoo.push_back( static_cast<const foo&>(b));
  b.pfoo = std::make_shared<foo>(static_cast<const foo&>(b));
  std::cout << json << std::endl;
  bar_json::serializer()(b, std::ostream_iterator<char>(std::cout) );
  std::cout << std::endl;
  
  // {"flag":true,"value":0,"string":"Привет Мир","vfoo":[],"pfoo":null}
  // {"flag":true,"value":0,"string":"Привет Мир","vfoo":[{"flag":true,"value":0,"string":"Привет Мир"}],"pfoo":{"flag":true,"value":0,"string":"Привет Мир"}}
}