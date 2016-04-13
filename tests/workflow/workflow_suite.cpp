#include <fas/testing.hpp>
#include <iow/workflow/workflow.hpp>
#include <chrono>

UNIT(workflow1, "")
{
  using namespace ::fas::testing;
  
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  
  ::iow::workflow queue(io, opt, 3, false);
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


BEGIN_SUITE(workflow, "")
  ADD_UNIT(workflow1)
END_SUITE(workflow)

