#include <iostream>

#if __cplusplus >= 201103L

#include <chrono>
#include <wjson/json.hpp>
#include <wjson/name.hpp>
#include <cstring>

#define TESTS 1000000

void str1_bench();


void str1_bench()
{
  using namespace std::chrono;
  using namespace wjson;

  std::cout << "String benchmark: " << std::endl;
  std::string str="Привет";
  std::string json;

  time_t stime = std::numeric_limits< time_t >::max();
  time_t dtime1 = std::numeric_limits< time_t >::max();
  time_t dtime2 = std::numeric_limits< time_t >::max();

  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (int i = 0; i < TESTS; i++)
    {
      json.clear();
      value<std::string>::serializer()( str, std::back_inserter(json) );
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( stime==0 || t < stime )
      stime = t;
  }

  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (int i = 0; i < TESTS; i++)
    {
      std::string().swap(str);
      value< std::string, 0 >::serializer()( str, std::begin(json), std::end(json), 0 );
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( dtime1==0 || t < dtime1 )
      dtime1 = t;
  }

  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (int i = 0; i < TESTS; i++)
    {
      std::string().swap(str);
      value< std::string, 40 >::serializer()( str, std::begin(json), std::end(json), 0 );
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( dtime2==0 || t < dtime2 )
      dtime2 = t;
  }

  std::cout << "serialization time: " << stime << " ns" << std::endl;
  std::cout << "serialization rate: " << size_t(( 1000000000.0f/float(stime) ) * TESTS) << " persec" << std::endl;
  std::cout << "deserialization time 1: " << dtime1 << " ns" << std::endl;
  std::cout << "deserialization rate 1: " << size_t(( 1000000000.0f/float(dtime1) ) * TESTS) << " persec" << std::endl;
  std::cout << "deserialization time 2: " << dtime2 << " ns" << std::endl;
  std::cout << "deserialization rate 2: " << size_t(( 1000000000.0f/float(dtime2) ) * TESTS) << " persec" << std::endl;
  std::cout << std::endl;
}

int main()
{
  str1_bench();
}

#else

int main()
{
  std::cout << "Benchmark for c++03 not impl" << std::endl;
}

#endif
