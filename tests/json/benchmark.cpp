#include <iow/json/json.hpp>
#include <iow/json/name.hpp>
#include <chrono>
#include <iostream>

struct foo
{
  char   field1 = 'A';
  int    field2 = 1234567;
  size_t field3 = 123456798000;
  std::string field4 = "Привет мир!";
  std::vector<int> field5={1,2,3,4,5};
};

struct foo_json
{
  JSON_NAME(field1)
  JSON_NAME(field2)
  JSON_NAME(field3)
  JSON_NAME(field4)
  JSON_NAME(field5)
  typedef ::iow::json::object<
    foo,
    ::iow::json::member_list<
      ::iow::json::member<n_field1, foo, char, &foo::field1>,
      ::iow::json::member<n_field2, foo, int, &foo::field2>,
      ::iow::json::member<n_field3, foo, size_t, &foo::field3>,
      ::iow::json::member<n_field4, foo, std::string, &foo::field4>,
      ::iow::json::member<n_field5, foo, std::vector<int>, &foo::field5, ::iow::json::array< std::vector< ::iow::json::value<int> > > >
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
};

const size_t SER_COUNT = 500;
const size_t ARR_SIZE = 10000000;
int main()
{
  using namespace std::chrono;
  typedef foo_json::serializer serializer;
  char* json = new char[ARR_SIZE];
  std::fill_n(json, ARR_SIZE, '\0' );
  foo f;
  std::vector<foo> vf;
  time_t stime = std::numeric_limits< time_t >::max();
  time_t dtime = std::numeric_limits< time_t >::max();
  size_t dcount = 0;
  ::iow::json::json_error e;
  for (int j=0; j < 1000; ++j)
  {
    char* beg = json;
    char* end = json + ARR_SIZE;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < SER_COUNT; ++i)
      beg = serializer()(f, beg );
    auto finish = high_resolution_clock::now();
    auto t = duration_cast<nanoseconds>(finish - start).count();
    if ( stime==0 || t < stime )
      stime = t;

    vf.resize(SER_COUNT);
    int fi = 0;
    start = high_resolution_clock::now();
    beg = json;
    end = json + ARR_SIZE;
    while ( beg!=end && *beg!='\0')
    {
      ++dcount;
      beg = serializer()(vf[fi++], beg, end, &e );
    }
    finish = high_resolution_clock::now();

    t = duration_cast<nanoseconds>(finish - start).count();
    if ( dtime==0 || t < dtime )
      dtime = t;
    
  }
  std::cout << json << std::endl;
  std::cout << "serialization time: " << stime << " ns" << std::endl;
  std::cout << "serialization rate: " << ( 1000000000/stime ) * SER_COUNT << " persec" << std::endl;
  std::cout << "deserialization time: " << dtime << " ns" << std::endl;
  std::cout << "deserialization rate: " << ( 1000000000/dtime ) * SER_COUNT << " persec" << std::endl;
  std::cout << "deserialization count: " << dcount << std::endl;
  return 0;
}