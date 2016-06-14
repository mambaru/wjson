#include <fas/testing.hpp>
#include <iow/workflow/asio_queue.hpp>
#include "delayed_common_suite.hpp"

UNIT(asio_queue1, "")
{
  ::iow::asio::io_service io;
  auto pq = std::make_shared< ::iow::asio_queue >(io, 0);
  delayed_unit1(t, *pq);
}


UNIT(asio_queue2, "")
{
  ::iow::asio::io_service io;
  auto pq = std::make_shared< ::iow::asio_queue >(io, 0);
  delayed_unit2(t, *pq);
}

UNIT(asio_queue3, "")
{
  ::iow::asio::io_service io;
  auto pq = std::make_shared< ::iow::asio_queue >(io, 0);
  delayed_unit3(t, *pq);
}

UNIT(asio_queue4, "")
{
  ::iow::asio::io_service io;
  auto pq = std::make_shared< ::iow::asio_queue >(io, 0);
  delayed_unit4(t, *pq);
}

BEGIN_SUITE(asio_queue, "")
  ADD_UNIT(asio_queue1)
  ADD_UNIT(asio_queue2)
  ADD_UNIT(asio_queue3)
  ADD_UNIT(asio_queue4)
END_SUITE(asio_queue)

