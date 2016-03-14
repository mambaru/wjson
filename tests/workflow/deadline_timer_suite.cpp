#include <fas/testing.hpp>
#include <iow/workflow/timed_queue.hpp>
#include "delayed_common_suite.hpp"

UNIT(timed_queue1, "")
{
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timed_queue >(io, opt);
  delayed_unit1(t, *pq);
}


UNIT(timed_queue2, "")
{
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timed_queue >(io, opt);
  delayed_unit2(t, *pq);
}

UNIT(timed_queue3, "")
{
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timed_queue >(io, opt);
  delayed_unit3(t, *pq);
}

UNIT(timed_queue4, "")
{
  ::iow::asio::io_service io;
  ::iow::queue_options opt;
  auto pq = std::make_shared< ::iow::timed_queue >(io, opt);
  delayed_unit4(t, *pq);
}

BEGIN_SUITE(timed_queue, "")
  ADD_UNIT(timed_queue1)
  ADD_UNIT(timed_queue2)
  ADD_UNIT(timed_queue3)
  ADD_UNIT(timed_queue4)
END_SUITE(timed_queue)

