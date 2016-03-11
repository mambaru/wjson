#include <fas/testing.hpp>
#include <iow/thread/timer_manager.hpp>
#include <iow/thread/delayed_queue.hpp>

UNIT(timer_manager1, "")
{
  using namespace ::fas::testing;
  auto pq = std::make_shared< ::iow::delayed_queue >();
  ::iow::timer_manager< ::iow::delayed_queue  > tm(pq);
  tm.create( std::chrono::milliseconds(200), [](){return true;} );
  t << nothing;
}


BEGIN_SUITE(timer_manager, "")
  ADD_UNIT(timer_manager1)
END_SUITE(timer_manager)

