#include <fas/testing.hpp>
#include <iow/thread/delayed_queue.hpp>
#include <chrono>
#include <atomic>
#include <thread>
#include <cmath>
#include <condition_variable>

UNIT(delayed_queue, "")
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  
  ::iow::delayed_queue dq;
  high_resolution_clock::time_point start = high_resolution_clock::now();
  high_resolution_clock::time_point finish = start;
  dq.delayed_post( milliseconds(200), [&t, &finish](){
    finish = high_resolution_clock::now();
    t << message("ready");
  });
  t << message("run_one: ") << dq.run_one();
  time_t ms = duration_cast<milliseconds>(finish-start).count();
  t << message("time: ") << ms;
  t << equal<assert>(ms, 200) << "delay fail. ms=" << ms << " should be 200";
}


UNIT(delayed_queue2, "")
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  
  ::iow::delayed_queue dq;
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

UNIT(delayed_queue3, "")
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  
  ::iow::delayed_queue dq;
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
    
  t << message( "stop...");
  dq.stop();
  th.join();
  
  t << equal<expect, int>(count, 10) << "count=" << count << " should be 10";
  for (int i=0 ; i < 5; ++i)
  {
    t << equal<expect>(time_ms[i], time_chk[i]) << "delay fail. ms=" << time_chk[i] << " should be " << time_ms[i];
  }
}

UNIT(delayed_queue4, "")
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  std::vector<time_t> times(10);
  high_resolution_clock::time_point start;
  ::iow::delayed_queue dq;
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
      //std::this_thread::sleep_for( milliseconds(100) );
      //t << message("thread run! id ") << std::this_thread::get_id() << " i=" << i ;
      dq.run();
    } ).detach();
  }
  t << message("go!");
  //std::this_thread::sleep_for( milliseconds(100) );
  start = high_resolution_clock::now();
  notify = true;
  cv.notify_all();
  std::this_thread::sleep_for( milliseconds(650) );
  dq.stop();
  std::this_thread::sleep_for( milliseconds(10) );
  for (int i = 0; i < 10 ; ++i)
  {
    time_t res = 50*(i+1) - times[i];
    t << is_true<expect>( std::abs(res) < 2 ) << " time error";
  }
}

BEGIN_SUITE(delayed_queue, "")
  ADD_UNIT(delayed_queue)
  ADD_UNIT(delayed_queue2)
  ADD_UNIT(delayed_queue3)
  ADD_UNIT(delayed_queue4)
END_SUITE(delayed_queue)

