#include <iostream>

#if __cplusplus >= 201103L

#include <wjson/json.hpp>
#include <wjson/name.hpp>
#include <chrono>
#include <cstring>

//const size_t NUM_SIZE = 20;
const int ARR_SIZE = 10000000;
typedef char numstr_t[20];
typedef numstr_t numarr_t[ARR_SIZE];
numarr_t numarr;
typedef int resarr_t[ARR_SIZE];
resarr_t resarr;

void json_bench();
void atoi_bench();
void ss_bench();


void json_bench()
{
  using namespace std::chrono;
  using namespace ::wjson;

  std::cout << "JSON benchmark: " << std::endl;
  memset( numarr, 0, sizeof(numarr) );
  memset( resarr, 0, sizeof(resarr) );

  
  time_t stime = std::numeric_limits< time_t >::max();
  time_t dtime = std::numeric_limits< time_t >::max();

  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (int i = 0; i < ARR_SIZE; i++)
    {
      value<int>::serializer()( i, numarr[i] );
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( stime==0 || t < stime )
      stime = t;
  }

  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (int i = 0; i < ARR_SIZE; i++)
    {
      value<int>::serializer()( resarr[i], std::begin(numarr[i]), std::end(numarr[i]), NULL );
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( dtime==0 || t < dtime )
      dtime = t;
  }

  /*for (int i = 0; i < ARR_SIZE; i++)
    std::cout << resarr[i] << " ";
  std::cout << std::endl;*/

  std::cout << "serialization time: " << stime << " ns" << std::endl;
  std::cout << "serialization rate: " << size_t(( 1000000000.0f/ float(stime) ) * ARR_SIZE) << " persec" << std::endl;
  std::cout << "deserialization time: " << dtime << " ns" << std::endl;
  std::cout << "deserialization rate: " << size_t(( 1000000000.0f/ float(dtime) ) * ARR_SIZE) << " persec" << std::endl;
  std::cout << std::endl;
}

void atoi_bench()
{
  using namespace std::chrono;
  using namespace wjson;

  std::cout << "sprintf/atoi benchmark: " << std::endl;
  memset( numarr, 0, sizeof(numarr) );
  memset( resarr, 0, sizeof(resarr) );

  time_t stime = std::numeric_limits< time_t >::max();
  time_t dtime = std::numeric_limits< time_t >::max();

  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < ARR_SIZE; i++)
    {
      sprintf( numarr[i], "%lu", i );
      //value<int>::serializer()( i, numarr[i] );
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( stime==0 || t < stime )
      stime = t;
  }

  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < ARR_SIZE; i++)
    {
      resarr[i] = atoi(numarr[i]);
      //value<int>::serializer()( resarr[i], std::begin(numarr[i]), std::end(numarr[i]), 0 );
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( dtime==0 || t < dtime )
      dtime = t;
  }

  /*for (int i = 0; i < ARR_SIZE; i++)
    std::cout << resarr[i] << " ";
  std::cout << std::endl;*/
    
  

  std::cout << "serialization time: " << stime << " ns" << std::endl;
  std::cout << "serialization rate: " << size_t(( 1000000000.0f/float(stime) ) * ARR_SIZE) << " persec" << std::endl;
  std::cout << "deserialization time: " << dtime << " ns" << std::endl;
  std::cout << "deserialization rate: " << size_t(( 1000000000.0f/float(dtime) ) * ARR_SIZE) << " persec" << std::endl;
  std::cout << std::endl;
}


/*
struct ostreambuf : public std::basic_streambuf<char, std::char_traits<char> >
{
  std::basic_streambuf<char, std::char_traits<char> >*
    setbuf(char_type* buffer, std::streamsize bufferLength)
  {
    return std::basic_streambuf<char, std::char_traits<char> >::setbuf(buffer, bufferLength);
    //this->setp(buffer, buffer + bufferLength);
  }
};*/

void ss_bench()
{
  using namespace std::chrono;
  using namespace wjson;


  std::cout << "stringstream benchmark: " << std::endl;
  
  memset( numarr, 0, sizeof(numarr) );
  memset( resarr, 0, sizeof(resarr) );

  /*for (int i = 0; i < ARR_SIZE; i++)
    std::cout << resarr[i] << " ";
  std::cout << std::endl;*/

  time_t stime = std::numeric_limits< time_t >::max();
  time_t dtime = std::numeric_limits< time_t >::max();

  std::stringstream ss;
  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < ARR_SIZE; i++)
    {
      ss.rdbuf()->pubsetbuf(numarr[i], sizeof(numarr[i]));
      ss << i;
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( stime==0 || t < stime )
      stime = t;
  }

  for (int x=0; x < 5; ++x) 
  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < ARR_SIZE; i++)
    {
      ss.rdbuf()->pubsetbuf(numarr[i], sizeof(numarr[i]) );
      ss >> resarr[i];
    }
    auto finish = high_resolution_clock::now();
    
    time_t t = duration_cast<nanoseconds>(finish - start).count();
    if ( dtime==0 || t < dtime )
      dtime = t;
  }

  /*
  for (int i = 0; i < ARR_SIZE; i++)
    std::cout << resarr[i] << " ";
  std::cout << std::endl;
  */

  std::cout << "serialization time: " << stime << " ns" << std::endl;
  std::cout << "serialization rate: " << size_t(( 1000000000.0f/float(stime) ) * ARR_SIZE) << " persec" << std::endl;
  std::cout << "deserialization time: " << dtime << " ns" << std::endl;
  std::cout << "deserialization rate: " << size_t(( 1000000000.0f/float(dtime) ) * ARR_SIZE) << " persec" << std::endl;
  std::cout << std::endl;
}

int main()
{
  std::cout << "Int to string and string to int benchmarks." << std::endl << std::endl;
  ss_bench();
  atoi_bench();
  json_bench();
}
#else

int main()
{
  std::cout << "Benchmark for c++03 not impl" << std::endl;
}

#endif
