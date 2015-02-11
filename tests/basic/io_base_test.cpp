
#include <iostream>
#include <sstream>

#include <iow/basic/basic_io.hpp>


int main()
{
  typedef std::function<void(std::string)> test_fun1;
  typedef std::function<void(std::unique_ptr<std::string>)> test_fun2;
  int test_count1 = 0;
  test_fun1 owner_test1;
  test_fun1 post_test1;
  test_fun1 strand_test1;

  int test_count2 = 0;
  test_fun2 owner_test2;

  {
    iow::basic_io<>::options_type opt;
    iow::basic_io<> iob( opt );
    
    test_fun1 orig_test1 = [&iob, &test_count1](std::string)                  { ++test_count1; };

    test_fun2 orig_test2 = [&iob, &test_count2](std::unique_ptr<std::string>) { ++test_count2; };
    
    
    owner_test1 = iob.owner_wrap( orig_test1 );
    post_test1  = iob.wrap_post( orig_test1 );
    strand_test1  = iob.wrap_strand( orig_test1 );
    owner_test2 = iob.owner_wrap( orig_test2 );
    
    iob.post( [owner_test1](){owner_test1("");} );
    post_test1("");
    strand_test1("");
    owner_test2(nullptr);
    iob.start();
    sleep(1);
    iob.reset(opt);
    owner_test1("");
    post_test1("");
    strand_test1("");
    owner_test2(nullptr);
    iob.stop();
  }
  owner_test1("");
  post_test1("");
  strand_test1("");
  owner_test2(nullptr);
  std::cout << test_count1 << std::endl;
  std::cout << test_count2 << std::endl;
  return 0;
}
