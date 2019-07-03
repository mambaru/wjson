#include <iostream>
#include <fas/testing.hpp>
#include <wjson/json.hpp>
#include <wjson/_json.hpp>
#include <wjson/name.hpp>
#include <wjson/strerror.hpp>

namespace {

struct count1
{
  typedef enum
  {
    one = 1,
    two = 2,
    three = 4,
    four = 8,
    five = 16,
    six  = 32
  } type;
};


struct foo
{
  int   flags ;
  int   flags2 ;
  int   counter2 ;
  int   counter3 ;

  count1::type counter ; 

  foo()
    : flags(0)
    , flags2(0)
    , counter2(0)
    , counter3(0)
    , counter(count1::one)
  {}
};


struct enum_list1
{
  JSON_NAME(one)
  JSON_NAME(two)
  JSON_NAME(three)
  JSON_NAME(four)
  JSON_NAME(five)
  JSON_NAME(six)

  typedef ::wjson::member_list<
      ::wjson::enum_value<n_one, count1::type, count1::one>,
      ::wjson::enum_value<n_two, count1::type, count1::two>,
      ::wjson::enum_value<n_three, count1::type, count1::three>,
      ::wjson::enum_value<n_four, count1::type, count1::four>,
      ::wjson::enum_value<n_five, count1::type, count1::five>,
      ::wjson::enum_value<n_six, count1::type, count1::six>
  > type;
};

struct enum_flags1
{
  JSON_NAME(one)
  JSON_NAME(two)
  JSON_NAME(three)
  JSON_NAME(four)
  JSON_NAME(five)
  JSON_NAME(six)

  typedef ::wjson::member_list<
      ::wjson::enum_value<n_one, int, static_cast<int>(count1::one)>,
      ::wjson::enum_value<n_two, int, static_cast<int>(count1::two)>,
      ::wjson::enum_value<n_three, int, static_cast<int>(count1::three)>,
      ::wjson::enum_value<n_four, int, static_cast<int>(count1::four)>,
      ::wjson::enum_value<n_five, int, static_cast<int>(count1::five)>,
      ::wjson::enum_value<n_six, int, static_cast<int>(count1::six)>
  > type;
};

struct enum_flags2
{
  JSON_NAME(one)
  JSON_NAME(two)
  JSON_NAME(three)
  JSON_NAME(four)
  JSON_NAME(five)
  JSON_NAME(six)
  JSON_NAME(seven)

  typedef ::wjson::member_list<
      ::wjson::enum_value<n_one, int, static_cast<int>(count1::one)>,
      ::wjson::enum_value<n_two, int, static_cast<int>(count1::two)>,
      ::wjson::enum_value<n_three, int, static_cast<int>(count1::three)>,
      ::wjson::enum_value<n_four, int, static_cast<int>(count1::four)>,
      ::wjson::enum_value<n_five, int, static_cast<int>(count1::five)>,
      ::wjson::enum_value<n_six, int, static_cast<int>(count1::six)>,
      ::wjson::enum_value<n_seven, int, 64>
  > type;
};


struct count_json
{
  typedef ::wjson::enumerator<count1::type, enum_list1::type, wjson::nonstrict_mode > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct count2_json
{
  typedef ::wjson::enumerator<int, enum_flags2::type > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

template<char Sep>
struct flags1_json
{
  typedef ::wjson::flags<int, enum_flags1::type, Sep, wjson::strict_mode > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

template<char Sep>
struct flags2_json
{
  typedef ::wjson::flags<int, enum_flags2::type, Sep, wjson::strict_mode> type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};


template<char Sep>
struct foo_json
{
  JSON_NAME(counter)
  JSON_NAME(counter2)
  JSON_NAME(flags)
  JSON_NAME(flags2)
  typedef ::wjson::object<
    foo,
    ::wjson::member_list<
      ::wjson::member<n_counter, foo, count1::type, &foo::counter, count_json>,
      ::wjson::member<n_counter2, foo, int, &foo::counter2, count2_json>,
      ::wjson::member<n_flags, foo, int, &foo::flags, flags1_json<Sep> >,
      ::wjson::member<n_flags2, foo, int, &foo::flags2, flags2_json<Sep> >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

} // namespace

UNIT(enum1, "")
{
  using namespace fas::testing;
  using namespace wjson;
  int flags = static_cast<int>(count1::one)
            | static_cast<int>(count1::three)
            | static_cast<int>(count1::six);
  int check = flags;
  typedef flags2_json<','>::serializer serializer_t;
  std::string json;
  serializer_t()(flags, std::back_inserter(json));
  t << message("json:") << json;
  t << equal<expect>(json, "[\"one\",\"three\",\"six\"]" ) << FAS_FL;
  flags = 0;
  ::wjson::json_error e;

  serializer_t()(flags, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) << FAS_FL;
  t << equal<expect, int>(flags, check ) << FAS_FL;

  flags = 0;  
  json = "[ \"three\", \"six\", \"one\" ]";
  serializer_t()(flags, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) << FAS_FL ;
  t << equal<expect, int>(flags, check ) << FAS_FL;

  flags = 0;  
  json = "\"three, six, one\"";
  serializer_t()(flags, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) << FAS_FL ;
  t << equal<expect, int>(flags, check ) << FAS_FL;

  
  flags = 0;  
  json = "[ \"three\", \"six\", 1 ]";
  serializer_t()(flags, json.begin(), json.end(), &e);
  t << is_true<assert>(e) << FAS_FL;
  t << stop;
  t << equal<expect>( e.code(), error_code::ExpectedOf )     
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) << FAS_FL ;

}

UNIT(enum2, "")
{
  using namespace fas::testing;
  using namespace wjson;
  
  int flags = static_cast<int>(count1::one)
            | static_cast<int>(count1::three)
            | static_cast<int>(count1::six);
  int check = flags;
  typedef flags2_json<'|'>::serializer serializer_t;
  std::string json;
  serializer_t()(flags, std::back_inserter(json));
  t << message("json:") << json;
  t << equal<expect>(json, "\"one|three|six\"" ) << FAS_FL;
  flags = 0;
  ::wjson::json_error e;

  serializer_t()(flags, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) << FAS_FL ;
  t << equal<expect, int>(flags, check ) << FAS_FL;

  flags = 0;  
  json = "\"    six |  three   |  one   \"";
  serializer_t()(flags, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) << FAS_FL ;
  t << equal<expect, int>(flags, check ) << FAS_FL;

  flags = 0;  
  json = "[\"six\",\"three\",\"one\"]";
  serializer_t()(flags, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) << FAS_FL ;
  t << equal<expect, int>(flags, check ) << FAS_FL;

  flags = 0;  
  json = "\"three |six | 1 \"";
  serializer_t()(flags, json.begin(), json.end(), &e);
  t << is_true<assert>(e) << FAS_FL;
  t << equal<expect>( e.code(), error_code::InvalidEnum ) << FAS_FL;
  t << message("error:") << ::wjson::strerror::message(e) << ": " << ::wjson::strerror::trace(e, json.begin(), json.end());
}


UNIT(enum3, "")
{
  using namespace fas::testing;
  foo f;
  
  f.counter = count1::four;
  f.flags   = static_cast<int>(count1::one)
            | static_cast<int>(count1::three)
            | static_cast<int>(count1::six);
  f.counter2 = 32 ;
  f.flags2  = static_cast<int>(count1::three);


  std::string json;
  foo_json<','>::serializer()(f, std::back_inserter(json));
  t << message("json:") << json;
  
  f = foo();
  ::wjson::json_error e;
  foo_json<','>::serializer()(f, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) ;
  t << equal<expect, int>( static_cast<int>(f.counter), static_cast<int>(count1::four) ) << FAS_FL;
  t << equal<expect, int>(f.counter2, 32 ) << FAS_FL;
  t << equal<expect, int>(f.flags, 37 ) << FAS_FL;
  t << equal<expect, int>(f.flags2, int(count1::three) ) << FAS_FL;
}

UNIT(enum4, "")
{
  using namespace fas::testing;
  foo f;
  
  f.counter = count1::four;
  f.flags   = static_cast<int>(count1::one)
            | static_cast<int>(count1::three)
            | static_cast<int>(count1::six);
  f.counter2 = 32 ;
  f.flags2  = static_cast<int>(count1::three);
  
  std::string json;
  foo_json<'+'>::serializer()(f, std::back_inserter(json));
  t << message("json:") << json;
  
  f = foo();
  ::wjson::json_error e;
  foo_json<'+'>::serializer()(f, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) ;
  t << equal<expect, int>( static_cast<int>(f.counter), static_cast<int>(count1::four) ) << FAS_FL;
  t << equal<expect, int>(f.counter2, 32 ) << FAS_FL;
  t << equal<expect, int>(f.flags, 37 ) << FAS_FL;
  t << equal<expect, int>(f.flags2, int(count1::three) ) << FAS_FL;
}

UNIT(enum5, "")
{
  using namespace fas::testing;
  std::string json="\"six1\"";
  count1::type result =  count1::type();
  t << equal<expect, int>(result, 0) << FAS_FL;
  ::wjson::json_error e;
  count_json::serializer()(result, json.begin(), json.end(), &e);
  t << is_false<assert>(e) 
    << ::wjson::strerror::message(e) << ": "
    << ::wjson::strerror::trace(e, json.begin(), json.end()) 
    << FAS_FL;
  t << equal<expect, int>(result, 0) << FAS_FL;
  
}

BEGIN_SUITE(enumerator, "enumerator")
  ADD_UNIT(enum1)
  ADD_UNIT(enum2)
  ADD_UNIT(enum3)
  ADD_UNIT(enum4)
  ADD_UNIT(enum5)
END_SUITE(enumerator)

