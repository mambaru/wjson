#include <fas/testing.hpp>
#include <iow/thread/deadline_timer.hpp>
#include "delayed_common_suite.hpp"

UNIT(deadline_timer1, "")
{
  ::iow::asio::io_service io;
  ::iow::deadline_timer dq(io);
  delayed_unit1(t, dq);
}


UNIT(deadline_timer2, "")
{
  ::iow::asio::io_service io;
  ::iow::deadline_timer dq(io);
  delayed_unit2(t, dq);
}

UNIT(deadline_timer3, "")
{
  ::iow::asio::io_service io;
  ::iow::deadline_timer dq(io);
  delayed_unit3(t, dq);
}

UNIT(deadline_timer4, "")
{
  ::iow::asio::io_service io;
  ::iow::deadline_timer dq(io);
  delayed_unit4(t, dq);
}

BEGIN_SUITE(deadline_timer, "")
  ADD_UNIT(deadline_timer1)
  ADD_UNIT(deadline_timer2)
  ADD_UNIT(deadline_timer3)
  ADD_UNIT(deadline_timer4)
END_SUITE(deadline_timer)

