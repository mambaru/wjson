#include <fas/testing.hpp>
#include <iow/thread/timer_manager.hpp>
#include <iow/thread/delayed_queue.hpp>
#include <iow/thread/timed_queue.hpp>

UNIT(timer_manager1, "")
{
  using namespace ::fas::testing;
  iow::asio::io_service io;
  iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timed_queue >(io, opt);
  auto tm = std::make_shared< ::iow::timer_manager< ::iow::timed_queue > >( pq );
  std::atomic<int> counter(0);
  tm->create( std::chrono::milliseconds(200), [&counter, &t]()
  {
    ++counter;
    t << message("tick1");
    return true;
  } );
  tm->create( std::chrono::milliseconds(400), [&counter, &t]()
  {
    ++counter;
    t << message("tick2");
    return true;
  } );
  
  for ( int i =0 ; i < 3; i++)
    io.run_one();
  
  t << equal< assert,int >( counter, 3 ) << "counter 3!=" << counter ;
}


BEGIN_SUITE(timer_manager, "")
  ADD_UNIT(timer_manager1)
END_SUITE(timer_manager)

