#include <fas/testing.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>
#include <wjson/strerror.hpp>

namespace {
  
struct foo
{
  int foo1;
  std::string foo2;
  std::vector<std::string> foo3;
  const std::vector<int>& get_foo4() const { return _foo4;}
  void set_foo4( const std::vector<int>& v ) { _foo4 = v;}
  foo(): foo1(0) {}
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
  
  typedef ::wjson::object<
    foo,
    ::wjson::member_list<
      ::wjson::member<n_foo1, foo, int, &foo::foo1>,
      ::wjson::member<n_foo2, foo, std::string, &foo::foo2>,
      ::wjson::member<n_foo3, foo, std::vector<std::string>, &foo::foo3, ::wjson::vector_of_strings<10> >,
      ::wjson::member_p<n_foo4, foo, std::vector<int>, foo4, ::wjson::array< std::vector< ::wjson::value<int> > > >
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
  typedef ::wjson::object<
    bar,
    ::wjson::member_list<
      ::wjson::base< foo_json >,
      ::wjson::member<n_bar1, bar, foo, &bar::bar1, foo_json>,
      ::wjson::member<n_bar2, bar, std::list<foo>, &bar::bar2, ::wjson::array< std::list<foo_json> > >,
      ::wjson::member<n_bar3, bar, std::map<std::string, foo>, &bar::bar3, ::wjson::dict< std::map< ::wjson::value< std::string >, foo_json  >  > >
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct foo_array_json
{
  typedef ::wjson::object_array<
    foo,
    foo_json::member_list
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct bar_array_json
{
  typedef ::wjson::object_array<
    bar,
    ::wjson::member_list<
      ::wjson::base< foo_json >,
      ::wjson::member_array<bar, foo, &bar::bar1, foo_array_json>,
      ::wjson::member_array<bar, std::list<foo>, &bar::bar2, ::wjson::array< std::list<foo_array_json> > >,
      ::wjson::member_array<bar, std::map<std::string, foo>, &bar::bar3, ::wjson::dict< std::map< ::wjson::value< std::string >, foo_array_json  >  > >
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

} // namespace

UNIT(object1, "")
{
  using namespace fas::testing;
  bar b1;
  bar b2;
  b1.foo1 = +100500;
  b1.foo2 = "aqualung";
  b1.bar1.foo1 = -100500;
  b1.bar1.foo2 = "drowned";
  b1.bar2.push_back( b1 );
  b1.bar2.push_back( b1.bar1 );
  b1.bar3["Ingeborga"] = b1;
  b1.bar3["Dapkūnaitė"] = b1.bar1;

#if __cplusplus >= 201103L
  b1.foo3 = {"one", "two", "three"};
  b1.bar1.foo3 = {"three", "two", "one"};
  b1.set_foo4({1,2,3,4,5});
#endif

  std::string json1, json2;
  bar_json::serializer()( b1, std::back_inserter(json1) );
  t << message("json: ") << json1;
  bar_json::serializer()( b2, json1.begin(), json1.end(), 0 );
  bar_json::serializer()( b2, std::back_inserter(json2) );
  t << equal<expect>(json1, json2) << FAS_FL;
}

UNIT(object2, "")
{
  using namespace fas::testing;
  bar b1;
  bar b2;
  b1.foo1 = +100500;
  b1.foo2 = "aqualung";
  b1.bar1.foo1 = -100500;
  b1.bar1.foo2 = "drowned";
  
  b1.bar2.push_back( b1 );
  b1.bar2.push_back( b1.bar1 );
  b1.bar3["Ingeborga"] = b1;
  b1.bar3["Dapkūnaitė"] = b1.bar1;
  
#if __cplusplus >= 201103L
  b1.foo3 = {"one", "two", "three"};
  b1.bar1.foo3 = {"three", "two", "one"};
  b1.set_foo4({1,2,3,4,5});
#endif

  std::string json1, json2;
  bar_array_json::serializer()( b1, std::back_inserter(json1) );
  t << message("json: ") << json1;
  bar_array_json::serializer()( b2, json1.begin(), json1.end(), 0 );
  bar_array_json::serializer()( b2, std::back_inserter(json2) );
  t << equal<expect>(json1, json2) << FAS_FL;
}

namespace {
  
struct baz
{
  int foo1;
  std::string foo2;
  baz(): foo1(0) {};
};

template<bool SecondPriority>
struct baz_json
{
  JSON_NAME(foo)
  typedef ::wjson::object<
    baz,
    ::wjson::member_list
    <
      ::wjson::member_if
      <
        ::wjson::member<n_foo, baz, int, &baz::foo1>,
        ::wjson::member<n_foo, baz, std::string, &baz::foo2>,
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
  t << equal<expect>( json, "{\"foo\":\"hello\"}" ) << FAS_FL;
#if __cplusplus >= 201103L
  t << equal<expect>( json, "{'foo':'hello'}"_wjson ) << FAS_FL;
#endif
  b.foo2.clear();
  b.foo1=10;
  ::wjson::json_error e;
  typename baz_json<Pri>::serializer()(b, json.begin(), json.end(), &e );
  if ( e ) t << message("ERROR:") << ::wjson::strerror::message(e);
  t << equal<expect>( b.foo2, "hello" ) << FAS_FL;
  t << equal<expect>( b.foo1, 0 ) << FAS_FL;
  
  json.clear();
  b.foo1 = 123;
  b.foo2 = "hello";
  typename baz_json<Pri>::serializer()(b, std::back_inserter(json) );
  t << equal<expect>( json, "{\"foo\":123}" ) << FAS_FL;
  b.foo2="---";
  b.foo1=10;
  e.reset();
  typename baz_json<Pri>::serializer()(b, json.begin(), json.end(), &e );
  if ( e ) t << message("ERROR:") << ::wjson::strerror::message(e);
  t << equal<expect>( b.foo2, "" ) << FAS_FL;
  t << equal<expect>( b.foo1, 123 ) << FAS_FL;
}

} //namespace

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

