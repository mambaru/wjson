#include <fas/testing.hpp>
#include <iow/thread/timer_queue.hpp>
#include "delayed_common_suite.hpp"

UNIT(timer_queue1, "")
{
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timer_queue >(io, opt);
  delayed_unit1(t, *pq);
}


UNIT(timer_queue2, "")
{
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timer_queue >(io, opt);
  delayed_unit2(t, *pq);
}

UNIT(timer_queue3, "")
{
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timer_queue >(io, opt);
  delayed_unit3(t, *pq);
}

UNIT(timer_queue4, "")
{
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timer_queue >(io, opt);
  delayed_unit4(t, *pq);
}

BEGIN_SUITE(timer_queue, "")
  ADD_UNIT(timer_queue1)
  ADD_UNIT(timer_queue2)
  ADD_UNIT(timer_queue3)
  ADD_UNIT(timer_queue4)
END_SUITE(timer_queue)

