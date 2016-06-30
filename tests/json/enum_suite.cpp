#include <fas/testing.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace {
  
enum class count1: int
{
  one   = 1,
  two   = 2,
  three = 4,
  four  = 8,
  five  = 16,
  six   = 32
};



struct count2
{
  typedef enum: int
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
  int   flags = 0;
  int   flags2 = 0;
  int   counter2 = 0;
  int   counter3 = 0;
  count1 counter = count1::one; 
};


struct enum_list1
{
  JSON_NAME(one)
  JSON_NAME(two)
  JSON_NAME(three)
  JSON_NAME(four)
  JSON_NAME(five)
  JSON_NAME(six)

  typedef ::iow::json::member_list<
      ::iow::json::enum_value<n_one, count1, count1::one>,
      ::iow::json::enum_value<n_two, count1, count1::two>,
      ::iow::json::enum_value<n_three, count1, count1::three>,
      ::iow::json::enum_value<n_four, count1, count1::four>,
      ::iow::json::enum_value<n_five, count1, count1::five>,
      ::iow::json::enum_value<n_six, count1, count1::six>
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

  typedef ::iow::json::member_list<
      ::iow::json::enum_value<n_one, int, static_cast<int>(count1::one)>,
      ::iow::json::enum_value<n_two, int, static_cast<int>(count1::two)>,
      ::iow::json::enum_value<n_three, int, static_cast<int>(count1::three)>,
      ::iow::json::enum_value<n_four, int, static_cast<int>(count1::four)>,
      ::iow::json::enum_value<n_five, int, static_cast<int>(count1::five)>,
      ::iow::json::enum_value<n_six, int, static_cast<int>(count1::six)>
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

  typedef ::iow::json::member_list<
      ::iow::json::enum_value<n_one, int, static_cast<int>(count1::one)>,
      ::iow::json::enum_value<n_two, int, static_cast<int>(count1::two)>,
      ::iow::json::enum_value<n_three, int, static_cast<int>(count1::three)>,
      ::iow::json::enum_value<n_four, int, static_cast<int>(count1::four)>,
      ::iow::json::enum_value<n_five, int, static_cast<int>(count1::five)>,
      ::iow::json::enum_value<n_six, int, static_cast<int>(count1::six)>,
      ::iow::json::enum_value<n_seven, int, 64>
  > type;
};


struct count_json
{
  typedef ::iow::json::enumerator<count1, enum_list1::type > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct count2_json
{
  typedef ::iow::json::enumerator<int, enum_flags2::type > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct flags1_json
{
  typedef ::iow::json::flags_enumerator<int, enum_flags1::type > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct flags2_json
{
  typedef ::iow::json::flags_enumerator<int, enum_flags2::type > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


struct foo_json
{
  JSON_NAME(counter)
  JSON_NAME(counter2)
  JSON_NAME(flags)
  JSON_NAME(flags2)
  typedef ::iow::json::object<
    foo,
    ::iow::json::member_list<
      ::iow::json::member<n_counter, foo, count1, &foo::counter, count_json>,
      ::iow::json::member<n_counter2, foo, int, &foo::counter2, count2_json>,
      ::iow::json::member<n_flags, foo, int, &foo::flags, flags1_json>,
      ::iow::json::member<n_flags2, foo, int, &foo::flags2, flags2_json>
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

} // namespace

UNIT(enum1, "")
{
  using namespace fas::testing;
  foo f;
  
  f.counter = count1::four;
  f.flags   = static_cast<int>(count1::one)
            | static_cast<int>(count1::three)
            | static_cast<int>(count1::six);
  f.counter2 = 63;
            
  std::string json;
  foo_json::serializer()(f, std::back_inserter(json));
  t << message("json:") << json;
  
  f = foo();
  iow::json::json_error e;
  foo_json::serializer()(f, json.begin(), json.end(), &e);
}

BEGIN_SUITE(enumerator, "enumerator")
  ADD_UNIT(enum1)
END_SUITE(enumerator)

