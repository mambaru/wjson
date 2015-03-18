#include <iostream>
#include <iow/io/io_base.hpp>
#include <iow/io/basic/aspect.hpp>
#include <fas/testing.hpp>


class foo
{
public:
  virtual ~foo(){};
  foo():counter(0){std::cout << "foo()" << std::endl;}
  foo(const foo& f){std::cout << "foo(const foo&)" << std::endl; counter = f.counter + 1;}
  foo(foo&& f){std::cout << "foo(foo&&)" << std::endl; counter = f.counter + 1;}
  void operator=( foo& f){std::cout << "=(foo&)" << std::endl; counter = f.counter + 1;}
  void operator=( foo&& f){std::cout << "=(foo&&)" << std::endl; counter = f.counter + 1;}
  void doit() const{ std::cout << "counter: " << counter << std::endl;}
  int counter;
};

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
      ::iow::io::basic::aspect<>::advice_list,
      fas::advice<_start_, ad_start>,
      fas::advice<_init_,  ad_init>,
      fas::advice<_stop_,  ad_stop>,
      fas::advice<_reset_, ad_reset>,
      fas::group< ::iow::io::_initialize_, _init_>,
      fas::group< ::iow::io::_after_stop_, _stop_>,
      fas::group< ::iow::io::_after_reset_, _reset_>,
      fas::group< ::iow::io::_after_start_, _start_>
    > >
{

public:
  void start(int val) { this->start_(*this, val ); }
  void stop() { this->stop_(*this); }
  void reset() { this->reset_(*this); }
  void initialize(int val) { this->initialize_(*this, val ); }
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
  io.initialize( 10 );
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
  bool flag = false;
  typedef std::function<void()> handler_type;
  
  foo f;
  int counter = 0;
  auto test1= io.wrap( [&flag, &counter, f](bool val)
  {
    flag = val;
    counter = f.counter;
  } );
  
  test1(true);
  t << is_true< assert >(counter==1) << counter<< FAS_TESTING_FILE_LINE;
  t << is_true< assert >(flag) << FAS_TESTING_FILE_LINE;
  io.reset();
  test1(false);
  t << is_true< assert >(counter==1) << FAS_TESTING_FILE_LINE;
  t << is_true< assert >(flag) << FAS_TESTING_FILE_LINE;
  t << nothing;
  
  std::cout << "id: " << io.get_id() << std::endl;
  io.start(20);
  io.stop();
  t << is_true< assert >(io.get_id()==2) << FAS_TESTING_FILE_LINE;
}

BEGIN_SUITE(io_base,"")
  ADD_UNIT(init)
  ADD_UNIT(start)
  ADD_UNIT(owner)
END_SUITE(io_base)

