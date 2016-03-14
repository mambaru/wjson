#include <fas/testing.hpp>

BEGIN_TEST
  RUN_SUITE(delayed_queue)
  RUN_SUITE(timed_queue)
  RUN_SUITE(timer_manager)
  RUN_SUITE(thread_pool)
  RUN_SUITE(dataflow)
END_TEST
