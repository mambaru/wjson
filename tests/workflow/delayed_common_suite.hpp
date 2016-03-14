#pragma once

#include <fas/testing.hpp>
#include <iow/workflow/delayed_queue.hpp>
#include <chrono>
#include <atomic>
#include <thread>
#include <cmath>
#include <condition_variable>

template<typename T, typename Q>
inline void delayed_unit1(T& t, Q& dq)
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  
  high_resolution_clock::time_point start = high_resolution_clock::now();
  high_resolution_clock::time_point finish = start;
  dq.delayed_post( milliseconds(200), [&t, &finish](){
    finish = high_resolution_clock::now();
  });
  dq.run_one();
  time_t ms = duration_cast<milliseconds>(finish-start).count();
  t << message("time: ") << ms;
  t << equal<assert>(ms, 200) << "delay fail. ms=" << ms << " should be 200";
}


template<typename T, typename Q>
inline void delayed_unit2(T& t, Q& dq)
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  
  high_resolution_clock::time_point start = high_resolution_clock::now();
  high_resolution_clock::time_point finish = start;
  dq.delayed_post( milliseconds(200), [&t, &finish](){
    finish = high_resolution_clock::now();
    t << message("ready delayed_post");
  });
  dq.post([&t, &finish](){
    finish = high_resolution_clock::now();
    t << message("ready post");
  });
  t << message("run_one: ") << dq.run_one();
  t << message("run_one: ") << dq.run_one();
  time_t ms = duration_cast<milliseconds>(finish-start).count();
  t << message("time: ") << ms;
  t << equal<assert>(ms, 200) << "delay fail. ms=" << ms << " should be 200";
}

template<typename T, typename Q>
inline void delayed_unit3(T& t, Q& dq)
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  
  time_t time_ms[] = {100, 200, 300, 400, 500};
  time_t time_chk[] = { 0,   0,   0,   0,   0};
  std::atomic<size_t> count;
  count = 0;
  auto start = high_resolution_clock::now();
  for (int i=0 ; i < 5; ++i)
  {
    dq.post([&count](){ ++count; });
    time_t& tm = time_chk[i];
    dq.delayed_post(milliseconds(time_ms[i]), [i, start, &count, &tm]()
    { 
      ++count; 
      tm = duration_cast<milliseconds>( high_resolution_clock::now() - start ).count();
    });
  }
  t << message( "start thread... ") << count;
  
  std::thread th([&dq, &t, &start](){
    t << message( "thread started ") ;
    start = high_resolution_clock::now();
    dq.run();
  });
  t << message( "while count ... ") << count;
  while (count < 10)
    std::this_thread::sleep_for( milliseconds(10)  );
    
  t << message( "stop... ") << count ;
  dq.stop();
  th.join();
  
  t << equal<expect, int>(count, 10) << "count=" << count << " should be 10";
  for (int i=0 ; i < 5; ++i)
  {
    t << equal<expect>(time_ms[i], time_chk[i]) << "delay fail. ms=" << time_chk[i] << " should be " << time_ms[i];
  }
}

template<typename T, typename Q>
inline void delayed_unit4(T& t, Q& dq)
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  std::vector<time_t> times(10);
  high_resolution_clock::time_point start;
  for (int i = 0; i < 10 ; ++i)
  {
    
    dq.delayed_post( milliseconds(50*(i+1)),[&t, &start, &times, i](){
      time_t& ms = times[i];
      ms = duration_cast<milliseconds>( high_resolution_clock::now() - start ).count();
      //t << message("ready! id=") << std::this_thread::get_id() << " ms=" << ms << " sleep " << (500-ms + 50);
      //std::this_thread::sleep_for( milliseconds(500-ms+50) );
    } );
  }
  
  std::condition_variable cv;
  
  bool notify = false;
  for (int i = 0; i < 4 ; ++i)
  {
    std::thread( [&](){
      std::mutex m;
      std::unique_lock< std::mutex> lck(m);
      cv.wait(lck, [&]{ return !notify;});
      dq.run();
    } ).detach();
  }
  
  start = high_resolution_clock::now();
  notify = true;
  cv.notify_all();
  std::this_thread::sleep_for( milliseconds(650) );
  dq.stop();
  std::this_thread::sleep_for( milliseconds(10) );
  for (int i = 0; i < 10 ; ++i)
  {
    time_t res = 50*(i+1) - times[i];
    t << is_true<expect>( std::abs(res) < 2 ) << " time error ( " << 50*(i+1) << "-" << times[i] << " > 1 )" ;
  }
}
