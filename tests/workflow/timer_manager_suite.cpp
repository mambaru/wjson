#include <fas/testing.hpp>
#include <iow/workflow/timer_manager.hpp>
#include <iow/workflow/delayed_queue.hpp>
#include <iow/workflow/asio_queue.hpp>
#include <iow/memory.hpp>

struct test_request
{
  typedef std::unique_ptr<test_request> ptr;
};

struct test_response
{
  typedef std::unique_ptr<test_response> ptr;
  typedef std::function<void(ptr)> handler;
};

class test_class
{
  void method( test_request::ptr, test_response::handler handler)
  {
    handler( std::make_unique<test_response>() );
  }
};


UNIT(timer_manager1, "")
{
  using namespace ::fas::testing;
  iow::asio::io_service io;
  iow::queue_options opt;
  auto pq = std::make_shared< ::iow::asio_queue >(io, opt);
  auto tm = std::make_shared< ::iow::timer_manager< ::iow::asio_queue > >( pq );
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
  
  auto tc = std::make_shared<test_class>();
  tm->create<test_request, test_response>( "", std::chrono::milliseconds(600), tc, &test_class::method, 
              [&t, &counter]( test_response::ptr) -> test_request::ptr 
              {
                ++counter;
                t << message("call1");
                return std::make_unique<test_request>();
              });
  
  for ( int i =0 ; i < 4; i++)
    io.run_one();
  
  t << equal< assert,int >( counter, 4 ) << "counter 4!=" << counter ;
}


BEGIN_SUITE(timer_manager, "")
  ADD_UNIT(timer_manager1)
END_SUITE(timer_manager)

