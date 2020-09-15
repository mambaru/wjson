#include <fas/testing.hpp>
#include <wjson/_json.hpp>
#include <wjson/schema.hpp>
#include <string>
#include <vector>

struct foo0
{
  int bar;

  foo0(): bar(0){}
  static foo0 create_schema() { return foo0();}
  static void create_schema(foo0& val) { val=foo0();}
  static foo0 create_schema(const std::string&) { return foo0();}
  static bool create_schema(foo0& val, const std::string&) { val=foo0(); return true;}
  static std::vector<std::string> get_schema_list() { return std::vector<std::string>();}
};


struct foo1
{
  int bar;
  foo1(): bar(0){}
};


struct foo2
{
  int bar;
  foo2(): bar(0){}
  static foo2 create_schema(const std::string&) { return foo2();}
};

struct foo3
{
  int bar;
  foo3(): bar(0){}
  static foo3 create_schema(const std::string&) { return foo3();}
  static std::vector<std::string> schema_list() { return std::vector<std::string>();}
};

struct foo4
{
  int bar;
  foo4(): bar(0){}
  static bool create_schema(foo4& f, const std::string& name )
  {
    if ( name == "1" ) f.bar = 1;
    else if ( name == "2" ) f.bar = 2;
    else if (name.empty()) f.bar = 3;
    else { f.bar = 4; return false; }
    return true;
  }
  static std::vector<std::string> schema_list()
  {
    std::vector<std::string> l;
    l.push_back("1"); l.push_back("2"); l.push_back("X");
    return l;
  }
};


UNIT(has_schema, "")
{
  using namespace fas::testing;
  t << is_true<expect>( wjson::schema::has_schema1<foo0>()) << FAS_FL;
  t << is_true<expect>( wjson::schema::has_schema2<foo0>()) << FAS_FL;
  t << is_true<expect>( wjson::schema::has_schema3<foo0>()) << FAS_FL;
  t << is_true<expect>( wjson::schema::has_schema4<foo0>()) << FAS_FL;
  t << is_true<expect>( wjson::schema::has_schema1<foo0>()) << FAS_FL;
  t << is_true<expect>( wjson::schema::has_schema<foo0>()) << FAS_FL;
  t << is_true<expect>( wjson::schema::has_get_schema_list<foo0>()) << FAS_FL;

  t << is_false<expect>( wjson::schema::has_schema<foo1>()) << FAS_FL;
  t << is_true<expect>( wjson::schema::has_schema<foo2>()) << FAS_FL;
  t << is_true<expect>( wjson::schema::has_schema<foo3>()) << FAS_FL;
}

UNIT(create_schema, "")
{
  using namespace fas::testing;
  foo0 f0;
  t << is_true<expect>( wjson::schema::create_schema<foo0>(f0,"")) << FAS_FL;
  foo1 f1;
  t << is_true<expect>( wjson::schema::create_schema<foo1>(f1,"")) << FAS_FL;

  foo4 f4;
  t << is_false<expect>( wjson::schema::create_schema<foo4>(f4,"5")) << FAS_FL;
  t << equal<expect>( f4.bar, 4) << FAS_FL;
  t << is_true<expect>( wjson::schema::create_schema<foo4>(f4,"")) << FAS_FL;
  t << equal<expect>( f4.bar, 3) << FAS_FL;
  t << is_true<expect>( wjson::schema::create_schema<foo4>(f4,"1")) << FAS_FL;
  t << equal<expect>( f4.bar, 1) << FAS_FL;
  t << is_true<expect>( wjson::schema::create_schema<foo4>(f4,"2")) << FAS_FL;
  t << equal<expect>( f4.bar, 2) << FAS_FL;

}

BEGIN_SUITE(schema, "")
  ADD_UNIT(has_schema)
  ADD_UNIT(create_schema)
END_SUITE(schema)
