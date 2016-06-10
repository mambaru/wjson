#include <fas/testing.hpp>
#include <chrono>

UNIT(rrqueue1, "")
{
  using namespace ::fas::testing;
  t << nothing;
  /*
  ::iow::circular_buffer<char> rr;
  t << equal<assert, size_t>(rr.size(), 0)  << FAS_TESTING_FILE_LINE;
  //t << equal<assert, size_t>(rr.max_size(), 0) << " max_size=" << rr.max_size() << FAS_TESTING_FILE_LINE;
  rr.set_capacity(1);
  rr.push_back('1');
  rr.push_back('0');
  t << equal<assert, size_t>(rr.size(), 1)  << rr.size() << " " << FAS_TESTING_FILE_LINE;
  */
  
}


BEGIN_SUITE(rrqueue, "")
  ADD_UNIT(rrqueue1)
END_SUITE(rrqueue)

