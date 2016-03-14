#include <fas/testing.hpp>
#include <iow/workflow/delayed_queue.hpp>
#include <iow/workflow/thread_pool.hpp>
#include <chrono>
#include <atomic>
#include <thread>
#include <cmath>
#include <condition_variable>

UNIT(thread_pool1, "")
{
  using namespace ::fas::testing;
  using namespace ::std::chrono;
  t << nothing;
}

BEGIN_SUITE(thread_pool, "")
  ADD_UNIT(thread_pool1)
END_SUITE(thread_pool)

