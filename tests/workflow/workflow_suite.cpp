#include <fas/testing.hpp>
#include <iow/workflow/task_manager.hpp>
#include <iow/workflow/bique.hpp>
#include <iow/workflow/workflow.hpp>
#include <chrono>

UNIT(workflow1, "")
{
  using namespace ::fas::testing;
  
  ::iow::asio::io_service io;

  ::iow::task_manager queue(io, 0, 3, false);
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

UNIT(workflow2, "")
{
  using namespace ::fas::testing;
  t << message("Покхали" );
  ::iow::asio::io_service io;  
  ::iow::asio::io_service::work wrk(io);
  ::iow::workflow* pw = nullptr;
  ::iow::workflow_options opt;
  bool ready = false;
  opt.maxsize = 5; // + handler timer
  opt.use_io_service = true;
  opt.threads = 1;
  opt.show_wrn_ms = 1000;
  opt.id = "test";
  opt.handler = [&pw, &ready, &io]()->bool 
  {
    std::cout << "]]]] " << pw->queue_size() << std::endl;
    if ( ready )
      return false;
    ready = pw->queue_size() == 1;
    if ( !ready )
      return true;
    
    io.stop();
    return false;
  };
  ::iow::workflow wfl(io, opt);
  pw = &wfl;
  wfl.start();
  for (int i =0 ; i < 5; i++)
    wfl.post( std::chrono::milliseconds(i*1000 + 1000),  [](){std::cout << "<->" << std::endl;});
  io.run();
  t << is_true<expect>(ready) << FAS_TESTING_FILE_LINE;
  t << is_true<expect>(pw->dropped()==1) << FAS_TESTING_FILE_LINE;
  t << is_true<expect>( pw->timer_count()==1 ) << FAS_TESTING_FILE_LINE;
  t << is_true<expect>( pw->queue_size()==1  ) << pw->queue_size() << " " << FAS_TESTING_FILE_LINE;
}

BEGIN_SUITE(workflow, "")
  ADD_UNIT(workflow1)
  ADD_UNIT(workflow2)
END_SUITE(workflow)

