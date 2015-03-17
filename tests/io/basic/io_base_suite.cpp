#include <iow/io/io_base.hpp>
#include <iow/io/basic/aspect.hpp>
#include <fas/testing.hpp>


typedef ::iow::io::io_base< fas::aspect< ::iow::io::basic::aspect<>::advice_list > > io_base;

UNIT(owner, "")
{
  using namespace fas::testing;
  
  io_base::io_service_type service;
  io_base io(service);
  bool flag = false;
  typedef std::function<void()> handler_type;
  
  auto test1= io.owner_wrap([&flag](bool val)
  {
    std::cout << "set " << val << std::endl;
    flag = val;
  });
  test1(true);
  t << is_true< assert >(flag) << FAS_TESTING_FILE_LINE;
  io.reset();
  test1(false);
  t << is_true< assert >(flag) << FAS_TESTING_FILE_LINE;
  t << nothing;
}

BEGIN_SUITE(io_base,"")
  ADD_UNIT(owner)
END_SUITE(io_base)

