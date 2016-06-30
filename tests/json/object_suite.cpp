#include <fas/testing.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>
#include <iow/json/strerror.hpp>

struct foo
{
  int foo1 = 0;
  std::string foo2;
  std::vector<std::string> foo3;
  const std::vector<int>& get_foo4() const { return _foo4;}
  void set_foo4( const std::vector<int>& v ) { _foo4 = v;}
private:
  std::vector<int> _foo4;
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
  JSON_NAME(foo4)
  
  struct foo4
  {
    void operator()(foo& f, const std::vector<int>& v) const 
    {
      f.set_foo4(v);
    }
    const std::vector<int>& operator()(const foo& f) const
    {
      return f.get_foo4();
    }
  };
  
  typedef ::iow::json::object<
    foo,
    ::iow::json::member_list<
      ::iow::json::member<n_foo1, foo, int, &foo::foo1>,
      ::iow::json::member<n_foo2, foo, std::string, &foo::foo2>,
      ::iow::json::member<n_foo3, foo, std::vector<std::string>, &foo::foo3, iow::json::vector_of_strings<10> >,
      ::iow::json::member_p<n_foo4, foo, std::vector<int>, foo4, iow::json::array< std::vector< iow::json::value<int> > > >
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

struct foo_array_json
{
  typedef ::iow::json::object_array<
    foo,
    foo_json::member_list
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct bar_array_json
{
  typedef ::iow::json::object_array<
    bar,
    ::iow::json::member_list<
      ::iow::json::base< foo_json >,
      ::iow::json::member_array<bar, foo, &bar::bar1, foo_array_json>,
      ::iow::json::member_array<bar, std::list<foo>, &bar::bar2, iow::json::array< std::list<foo_array_json> > >,
      ::iow::json::member_array<bar, std::map<std::string, foo>, &bar::bar3, iow::json::dict< std::map< iow::json::value< std::string >, foo_array_json  >  > >
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
  b1.set_foo4({1,2,3,4,5});

  std::string json1, json2;
  bar_json::serializer()( b1, std::back_inserter(json1) );
  t << message("json: ") << json1;
  bar_json::serializer()( b2, json1.begin(), json1.end(), 0 );
  bar_json::serializer()( b2, std::back_inserter(json2) );
  t << equal<expect>(json1, json2) << FAS_ENDL;
}

UNIT(object2, "")
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
  b1.set_foo4({1,2,3,4,5});
  
  std::string json1, json2;
  bar_array_json::serializer()( b1, std::back_inserter(json1) );
  t << message("json: ") << json1;
  bar_array_json::serializer()( b2, json1.begin(), json1.end(), 0 );
  bar_array_json::serializer()( b2, std::back_inserter(json2) );
  t << equal<expect>(json1, json2) << FAS_ENDL;
}

struct baz
{
  int foo1 = 0;
  std::string foo2;
};

template<bool SecondPriority>
struct baz_json
{
  JSON_NAME(foo)
  typedef ::iow::json::object<
    baz,
    ::iow::json::member_list
    <
      ::iow::json::member_if
      <
        ::iow::json::member<n_foo, baz, int, &baz::foo1>,
        ::iow::json::member<n_foo, baz, std::string, &baz::foo2>,
        SecondPriority
      >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};


template<bool Pri, typename T>
void object3_toster( T& t )
{
  using namespace fas::testing;
  baz b;
  std::string json;
  b.foo2 = "hello";

  typename baz_json<Pri>::serializer()(b, std::back_inserter(json) );
  t << equal<expect>( json, "{\"foo\":\"hello\"}" ) << FAS_ENDL;
  b.foo2.clear();
  b.foo1=10;
  iow::json::json_error e;
  typename baz_json<Pri>::serializer()(b, json.begin(), json.end(), &e );
  if ( e ) t << message("ERROR:") << iow::json::strerror::message(e);
  t << equal<expect>( b.foo2, "hello" ) << FAS_ENDL;
  t << equal<expect>( b.foo1, 0 ) << FAS_ENDL;
  
  json.clear();
  b.foo1 = 123;
  b.foo2 = "hello";
  typename baz_json<Pri>::serializer()(b, std::back_inserter(json) );
  t << equal<expect>( json, "{\"foo\":123}" ) << FAS_ENDL;
  b.foo2="---";
  b.foo1=10;
  e.reset();
  typename baz_json<Pri>::serializer()(b, json.begin(), json.end(), &e );
  if ( e ) t << message("ERROR:") << iow::json::strerror::message(e);
  t << equal<expect>( b.foo2, "" ) << FAS_ENDL;
  t << equal<expect>( b.foo1, 123 ) << FAS_ENDL;
}

UNIT(object3, "")
{
  object3_toster<true>(t);
  object3_toster<false>(t);
}

BEGIN_SUITE(object, "")
  ADD_UNIT(object1)
  ADD_UNIT(object2)
  ADD_UNIT(object3)
END_SUITE(object)

