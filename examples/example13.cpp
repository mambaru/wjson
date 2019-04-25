#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <fas/system/nullptr.hpp>
#include <iostream>

struct counter
{
  typedef enum
  {
    one = 1,
    two = 2,
    three = 3,
    four = 4,
    five = 5,
    six  = 6
  } type;
};

struct counter_json
{
  JSON_NAME(one)
  JSON_NAME(two)
  JSON_NAME(three)
  JSON_NAME(four)
  JSON_NAME(five)
  JSON_NAME2(n_six, "Привет мир!")

  typedef ::wjson::enumerator<
    counter::type,
    ::wjson::member_list<
      ::wjson::enum_value< n_one,  counter::type, counter::one>,
      ::wjson::enum_value< n_two,  counter::type, counter::two>,
      ::wjson::enum_value< n_three,counter::type, counter::three>,
      ::wjson::enum_value< n_four, counter::type, counter::four>,
      ::wjson::enum_value< n_five, counter::type, counter::five>,
      ::wjson::enum_value< n_six,  counter::type, counter::six>
    > 
  > type;

  typedef type::serializer serializer;
  typedef type::target target;
  typedef type::member_list member_list; 
};

int main()
{
  typedef ::wjson::array< std::vector< counter_json > > array_counter_json;

  std::vector< counter::type > cl;
  std::string json = "[\"one\",\"two\",\"three\"]";
  std::cout << json << std::endl;
  
  array_counter_json::serializer()( cl, json.begin(), json.end(), fas_nullptr );
  cl.push_back(counter::four);
  cl.push_back(counter::five);
  cl.push_back(counter::six);

  array_counter_json::serializer()(cl, std::ostream_iterator<char>(std::cout) );
  std::cout << std::endl;
  
  // ["one","two","three"]
  // ["one","two","three","four","five","Привет мир!"]
}
