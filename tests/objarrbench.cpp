#include <wjson/json.hpp>
#include <wjson/name.hpp>
#include <cstring>
#include <iostream>
#include <fas/system/nullptr.hpp>

#if __cplusplus >= 201103L
#include <chrono>

void sprintf_bench();

struct foo
{
  int field1 = 0;
  int field2 = 0;
  int field3 = 0;
  std::vector<int> field5={0,0,0,0,0};

  void init()
  {
    this->field1 = 10;
    this->field2 = 20;
    this->field3 = 30;
    this->field5={1,2,3,4,5};
  }

  bool check(const foo& f) const
  {
    return
       this->field1 == f.field1
    && this->field2 == f.field2
    && this->field3 == f.field3
    && this->field5 == f.field5;

  }
};

struct foo_json
{
  JSON_NAME(field1)
  JSON_NAME(field2)
  JSON_NAME(field3)
  JSON_NAME(field5)
  typedef ::wjson::object_array<
    foo,
    ::wjson::member_list<
      ::wjson::member<n_field1, foo, int, &foo::field1>,
      ::wjson::member<n_field2, foo, int, &foo::field2>,
      ::wjson::member<n_field3, foo, int, &foo::field3>,
      ::wjson::member<n_field5, foo, std::vector<int>, &foo::field5, ::wjson::array< std::vector< ::wjson::value<int> > > >
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
};

const size_t ARR_SIZE = 100000000;
#ifdef NDEBUG
const size_t TESTS = 500;
const size_t SER_COUNT = 1000;
#else
const size_t TESTS = 1;
const size_t SER_COUNT = 1;
#endif

void json_bench();

void json_bench()
{
  using namespace std::chrono;
  typedef foo_json::serializer serializer;
  char* json = new char[ARR_SIZE];
  std::fill_n(json, ARR_SIZE, '\0' );
  foo f;
  f.init();
  std::vector<foo> vf;
  time_t stime = std::numeric_limits< time_t >::max();
  time_t dtime = std::numeric_limits< time_t >::max();
  size_t dcount = 0;
  ::wjson::json_error e;
  for (size_t j=0; j < TESTS; ++j)
  {
    char* beg = json;
    char* end = json + ARR_SIZE;
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < SER_COUNT; ++i)
      beg = serializer()(f, beg );
    auto finish = high_resolution_clock::now();
    auto t = duration_cast<nanoseconds>(finish - start).count();
    if ( stime==0 || t < stime )
      stime = t;

    vf.resize(SER_COUNT);
    size_t fi = 0;
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

    for (size_t i = 0; i != vf.size(); ++i)
      if ( !f.check(vf[i]) )
        abort();
    std::vector<foo>().swap(vf);

  }
  //std::cout << json << std::endl;
  std::cout << "serialization time: " << stime << " ns" << std::endl;
  std::cout << "serialization rate: " << size_t( (1000000000.0f/float(stime) ) * SER_COUNT) << " persec" << std::endl;
  std::cout << "deserialization time: " << dtime << " ns" << std::endl;
  std::cout << "deserialization rate: " << size_t(( 1000000000.0f/float(dtime) ) * SER_COUNT) << " persec" << std::endl;
  std::cout << "deserialization count: " << dcount << std::endl;
  delete[] json;
}

void sprintf_bench()
{
  using namespace std::chrono;
  char* json = new char[ARR_SIZE];
  std::fill_n(json, ARR_SIZE, '\0' );
  foo f;
  f.init();
  std::vector<foo> vf;
  time_t stime = std::numeric_limits< time_t >::max();
  time_t dtime = std::numeric_limits< time_t >::max();
  size_t dcount = 0;
  ::wjson::json_error e;
  for (size_t j=0; j < TESTS; ++j)
  {
    memset(json, 0, ARR_SIZE);

    char* beg = json;
    char* end = json + ARR_SIZE;
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < SER_COUNT; ++i)
    {
      beg+=sprintf( beg, "[%d,%d,%d,[%d,%d,%d,%d,%d]]",
                   f.field1, f.field2, f.field3, f.field5[0], f.field5[1], f.field5[2], f.field5[3], f.field5[4]);
    }
    auto finish = high_resolution_clock::now();
    auto t = duration_cast<nanoseconds>(finish - start).count();
    if ( stime==0 || t < stime )
      stime = t;

    vf.clear();
    vf.resize(SER_COUNT);
    size_t fi = 0;
    start = high_resolution_clock::now();
    beg = json;
    end = json + ARR_SIZE;
    while ( beg!=end && *beg!='\0' && fi!=vf.size() )
    {
      auto& f2 = vf[fi++];
      ++dcount;
      // beg = [10,20,30,[1,2,3,4,5]] ....
      /*int sres = */sscanf( beg, "[%d,%d,%d,[%d,%d,%d,%d,%d]]",
               &(f2.field1), &(f2.field2), &(f2.field3), &(f2.field5[0]), &(f2.field5[1]), &(f2.field5[2]), &(f2.field5[3]), &(f2.field5[4]) );
      beg = ::wjson::parser::parse_array(beg, end, fas_nullptr);
    }
    finish = high_resolution_clock::now();

    t = duration_cast<nanoseconds>(finish - start).count();
    if ( dtime==0 || t < dtime )
      dtime = t;
    //for (const foo& v : vf)
    for (size_t i = 0; i != vf.size() ; ++i)
    {
      const foo& v = vf[i];
      if ( !f.check(v) )
      {
        std::cout << v.field1 << std::endl;
        std::cout << v.field2 << std::endl;
        std::cout << v.field3 << std::endl;
        std::cout << v.field5[0] << std::endl;
        std::cout << v.field5[1] << std::endl;
        std::cout << v.field5[2] << std::endl;
        std::cout << v.field5[3] << std::endl;
        std::cout << v.field5[4] << std::endl;
        //abort();
      }
    }
    vf.clear();
    std::vector<foo>().swap(vf);
  }

  std::cout << "serialization time: " << stime << " ns" << std::endl;
  std::cout << "serialization rate: " << size_t(( 1000000000.0f/float(stime) ) * SER_COUNT) << " persec" << std::endl;
  std::cout << "deserialization time: " << dtime << " ns" << std::endl;
  std::cout << "deserialization rate: " << size_t(( 1000000000.0f/float(dtime) ) * SER_COUNT) << " persec" << std::endl;
  std::cout << "deserialization count: " << dcount << std::endl;
  delete[] json;
}


int main()
{
  std::cout << std::endl<< "JSON benchmark:" << std::endl;
  json_bench();
  std::cout << std::endl<< "sprintf benchmark:" << std::endl;
  sprintf_bench();
}

#else

int main()
{
  std::cout << "Benchmark for c++03 not impl" << std::endl;
}

#endif

