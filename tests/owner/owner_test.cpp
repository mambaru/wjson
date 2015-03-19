#include <fas/testing.hpp>
#include <iow/owner/owner.hpp>
#include <boost/concept_check.hpp>

class foo
{
public:
  foo() {++ctor;}
  foo(const foo& ) {++cctor;}
  foo(foo&& ) {++mctor;}
public:
  static int ctor ;
  static int cctor ;
  static int mctor ;
};

int foo::ctor = 0;
int foo::cctor = 0;
int foo::mctor = 0;

int inc(int i)
{
  return i+1;
}

int dec(int i)
{
  return i-1;
}

struct finc{ int operator()(int val){ return val+1;}};
struct fdec{ int operator()(int dec){ return dec-1;}};

template<typename Handle>
void rec(Handle&& h, int i)
{
  if ( i == 0 ) h();
  else rec( std::forward<Handle>(h), i-1);
}

template<typename Handle>
void rec2(Handle h, int i)
{
  if ( i == 0 ) h();
  else rec( h, i-1);
}

UNIT(owner, "")
{
  using namespace fas::testing;
  ::iow::owner own;
  auto test1 = own.wrap(&inc);
  int val = 0;
  val = test1(1);
  t << equal<expect>(val, 2) << FAS_TESTING_FILE_LINE;
  own.reset();
  val = test1(1);
  t << equal<expect>(val, 0) << FAS_TESTING_FILE_LINE;
  
  auto test2 = own.wrap(&inc, &dec);
  val = test2(2);
  t << equal<expect>(val, 3) << FAS_TESTING_FILE_LINE;
  own.reset();
  val = test2(2);
  t << equal<expect>(val, 1) << FAS_TESTING_FILE_LINE;
  
  foo f;
  auto test3 = own.wrap([f](int val)->int { return val + 1;});
  val = test3(1);
  t << equal<expect>(val, 2) << FAS_TESTING_FILE_LINE;
  t << equal<expect>(foo::ctor, 1) << FAS_TESTING_FILE_LINE;
  t << equal<expect>(foo::cctor, 1) << foo::cctor << FAS_TESTING_FILE_LINE;
  t << equal<expect>(foo::mctor, 1) << FAS_TESTING_FILE_LINE;
  own.reset();
  val = test3(1);
  t << equal<expect>(val, 0) << FAS_TESTING_FILE_LINE;


  auto test4 = own.wrap([f](int val)->int { return val + 1;}, [f](int val)->int { return val - 1;});
  val = test4(2);
  t << equal<expect>(val, 3) << FAS_TESTING_FILE_LINE;
  own.reset();
  val = test4(2);
  t << equal<expect>(val, 1) << FAS_TESTING_FILE_LINE;

  t << equal<expect>(foo::ctor, 1) << FAS_TESTING_FILE_LINE;
  t << equal<expect>(foo::cctor, 3) << foo::cctor << FAS_TESTING_FILE_LINE;
  t << equal<expect>(foo::mctor, 3) << foo::mctor << FAS_TESTING_FILE_LINE;
  
  rec([f](){}, 10);
  t << equal<expect>(foo::cctor, 4) << foo::cctor << FAS_TESTING_FILE_LINE;
  t << equal<expect>(foo::mctor, 3) << foo::mctor << FAS_TESTING_FILE_LINE;

  rec2([f](){}, 10);
  t << equal<expect>(foo::cctor, 5) << foo::cctor << FAS_TESTING_FILE_LINE;
  t << equal<expect>(foo::mctor, 3) << foo::mctor << FAS_TESTING_FILE_LINE;
  
  auto test5 = test4;
  val = test5(2);
  t << equal<expect>(val, 1) << FAS_TESTING_FILE_LINE;

  auto test6 = own.wrap([f](int val)->int { return val + 1;});
  auto test7 = test6;
  val = test7(2);
  t << equal<expect>(val, 3) << FAS_TESTING_FILE_LINE;
  own.reset();
  val = test7(2);
  t << equal<expect>(val, 0) << FAS_TESTING_FILE_LINE;
  
  auto test8 = own.wrap(finc(), fdec());
  //auto test8 = own.wrap(&inc, &dec);
  //auto test8 = own.wrap([f](int val)->int { return val + 1;}, [f](int val)->int { return val - 1;});
  auto test9 = test8;
  val = test9(2);
  t << equal<expect>(val, 3) << val << FAS_TESTING_FILE_LINE;
  own.reset();
  val = test9(2);
  t << equal<expect>(val, 1) << val << FAS_TESTING_FILE_LINE;
  
}

BEGIN_SUITE(owner, "")
  ADD_UNIT(owner)
END_SUITE(owner)


BEGIN_TEST
  RUN_SUITE(owner)
END_TEST
