#include <fas/testing.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

struct foo
{
  int foo1 = 0;
  std::string foo2;
  std::vector<std::string> foo3;
};

struct bar: foo
{
  foo bar1;
  std::list<foo> bar2;
  std::map<std::string, foo> bar3;
};

struct foo_json
{
  JSON_NAME(foo1)
  JSON_NAME(foo2)
  JSON_NAME(foo3)
  
  typedef ::iow::json::object<
    foo,
    ::iow::json::member_list<
      ::iow::json::member<n_foo1, foo, int, &foo::foo1>,
      ::iow::json::member<n_foo2, foo, std::string, &foo::foo2>,
      ::iow::json::member<n_foo3, foo, std::vector<std::string>, &foo::foo3, iow::json::vector_of_strings<10> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct bar_json
{
  JSON_NAME(bar1)
  JSON_NAME(bar2)
  JSON_NAME(bar3)
  typedef ::iow::json::object<
    bar,
    ::iow::json::member_list<
      ::iow::json::base< foo_json >,
      ::iow::json::member<n_bar1, bar, foo, &bar::bar1, foo_json>,
      ::iow::json::member<n_bar2, bar, std::list<foo>, &bar::bar2, iow::json::array< std::list<foo_json> > >,
      ::iow::json::member<n_bar3, bar, std::map<std::string, foo>, &bar::bar3, iow::json::dict< std::map< iow::json::value< std::string >, foo_json  >  > >
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


UNIT(object1, "")
{
  using namespace fas::testing;
  bar b1;
  bar b2;
  b1.foo1 = +100500;
  b1.foo2 = "aqualung";
  b1.foo3 = {"one", "two", "three"};
  b1.bar1.foo1 = -100500;
  b1.bar1.foo2 = "drowned";
  b1.bar1.foo3 = {"three", "two", "one"};
  b1.bar2.push_back( b1 );
  b1.bar2.push_back( b1.bar1 );
  b1.bar3["Ingeborga"] = b1;
  b1.bar3["Dapkūnaitė"] = b1.bar1;
  
  std::string json1, json2;
  bar_json::serializer()( b1, std::back_inserter(json1) );
  t << message("json: ") << json1;
  bar_json::serializer()( b2, json1.begin(), json1.end(), 0 );
  bar_json::serializer()( b2, std::back_inserter(json2) );
  t << equal<expect>(json1, json2) << FAS_TESTING_FILE_LINE;
  
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  bar_json::serializer()( b1, std::ostreambuf_iterator<char>(std::cout) );
  std::cout << std::endl;
  std::cout << std::endl;
}

BEGIN_SUITE(object, "")
  ADD_UNIT(object1)
END_SUITE(object)

