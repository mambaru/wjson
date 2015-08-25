#include <iostream>
#include <iow/io/io_base.hpp>
#include <iow/io/basic/aspect.hpp>
#include <fas/testing.hpp>


struct _start_;
struct ad_start
{
  template<typename T>
  void operator() (T& t) 
  {
    std::cout << std::endl << "start" << std::endl;
    t.virt();
    t.started = true;
  }
};

struct _init_;
struct ad_init
{
  template<typename T>
  void operator() (T& t, int val)
  {
    std::cout << std::endl << "init" << std::endl;
    t.value = val;
    t.virt();
  }
};

struct _stop_;
struct ad_stop
{
  template<typename T>
  void operator() (T& t) 
  {
    t.started = false;
    t.virt();
  }
};

struct _reset_;
struct ad_reset
{
  template<typename T>
  void operator() (T& t) 
  {
    ++t.resetval;
    t.virt();
  }
};

class iobase
  : public ::iow::io::io_base< fas::aspect< 
      ::iow::io::basic::aspect< std::recursive_mutex >::advice_list,
      fas::advice<_start_, ad_start>,
      fas::advice<_init_,  ad_init>,
      fas::advice<_stop_,  ad_stop>,
      fas::advice<_reset_, ad_reset>,
      fas::group< ::iow::io::_initialize_, _init_>,
      fas::group< ::iow::io::_after_stop_, _stop_>,
      fas::group< ::iow::io::_after_reset_, _reset_>,
      fas::group< ::iow::io::_after_start_, _start_>,
      fas::type< ::iow::io::_options_type_, fas::empty_type >
    > >
{

public:
  void start(int val) { this->start_(*this, val ); }
  void stop() { this->stop_(*this); }
  void reset() { this->reset_(*this); }
  void reconfigure(int val) { this->reconfigure_(*this, val ); }
  virtual void virt() { ++virtval ;};
  int  value = -1;
  int  resetval = 0;
  int  virtval = 0;
  bool started = false;
  
};

UNIT(init, "")
{
  using namespace fas::testing;
  iobase io;
  io.reconfigure( 10 );
  t << is_true< assert >(io.virtval==1) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.resetval==0) << FAS_TESTING_FILE_LINE;
  t << is_false< assert >(io.started) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.value==10) << FAS_TESTING_FILE_LINE;
  t << is_false< assert >( io.status() ) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.get_id()==0) << FAS_TESTING_FILE_LINE;
  io.reset();
  t << is_true< assert >(io.virtval==2) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.resetval==1) << FAS_TESTING_FILE_LINE;
  t << is_false< assert >(io.started) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.value==10) << FAS_TESTING_FILE_LINE;
  t << is_false< assert >( io.status() ) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.get_id()==0) << FAS_TESTING_FILE_LINE;
}


UNIT(start, "")
{
  using namespace fas::testing;
  using namespace fas::testing;
  iobase io;
  io.start( 10 );
  t << is_true< assert >(io.virtval==2) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.resetval==0) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.started) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.value==10) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >( io.status() ) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.get_id()==1) << FAS_TESTING_FILE_LINE;
  io.stop();
  t << is_true< assert >(io.virtval==4) <<  FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.resetval==1) << FAS_TESTING_FILE_LINE;
  t << is_false< assert >(io.started) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.value==10) << FAS_TESTING_FILE_LINE;
  t << is_false< assert >( io.status() ) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(io.get_id()==1) << FAS_TESTING_FILE_LINE;
}


UNIT(owner, "")
{
  using namespace fas::testing;
  
  iobase io;
  int value = 0;
  auto test1= io.wrap([&value](int val) { value = val; });
  
  test1(10);
  t << is_true< assert >(value==10) << value << FAS_TESTING_FILE_LINE;
  io.reset();
  test1(20);
  t << is_true< assert >(value==10) << value << FAS_TESTING_FILE_LINE;

  auto test2= io.wrap([&value](int val) { value = val; }, [&value](int val) { value = val*-1; });

  test2(30);
  t << is_true< assert >(value==30) << value << FAS_TESTING_FILE_LINE;
  io.reset();
  test2(40);
  t << is_true< assert >(value==-40) << value << FAS_TESTING_FILE_LINE;
  
}

BEGIN_SUITE(io_base,"")
  ADD_UNIT(init)
  ADD_UNIT(start)
  ADD_UNIT(owner)
END_SUITE(io_base)

