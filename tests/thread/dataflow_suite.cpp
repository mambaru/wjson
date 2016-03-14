#include <fas/testing.hpp>
#include <iow/thread/dataflow.hpp>
#include <chrono>

UNIT(dataflow1, "")
{
  using namespace ::fas::testing;
  
  ::iow::asio::io_service io;
  ::iow::dataflow_options opt;
  opt.threads = 3;
  ::iow::dataflow queue(io, opt);
  queue.start();
  std::atomic<int> counter(0);
  queue.post([&t, &counter](){
    ++counter;
    t << message("post");
  });
  queue.timer()->create(std::chrono::milliseconds(400), [&t, &counter](){
    ++counter;
    t << message("timer 400ms");
    return true;
  });
  queue.delayed_post( std::chrono::milliseconds(600), [&t, &counter](){
    ++counter;
    t << message("delayed post 600ms");
  });
  for (int i =0 ; i < 3 ; ++i)
  {
    queue.delayed_post( std::chrono::milliseconds(300 + i*300), [&t, &counter, i](){
      ++counter;
      t << message("delayed post ") << 300 + i*300 << "ms";
    });
  }
  sleep(1);
  queue.stop();
  t << equal< assert,int >( counter, 7 ) << "counter 7!=" << counter ;
  t << nothing;
}


BEGIN_SUITE(dataflow, "")
  ADD_UNIT(dataflow1)
END_SUITE(dataflow)

