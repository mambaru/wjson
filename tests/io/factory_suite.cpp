#include <iostream>
#include <fas/testing.hpp>
#include <vector>
#include <memory>
#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

class barrier
{
private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::size_t _count;
public:
    explicit barrier(std::size_t count) : _count{count} { }
    void wait()
    {
        std::unique_lock<std::mutex> lock{_mutex};
        if (--_count == 0) {
            _cv.notify_all();
        } else {
            _cv.wait(lock, [this] { return _count == 0; });
        }
    }
};

barrier bar1(3);
barrier bar2(3);
static const size_t COUNT = 1000000;
static const size_t SIZE = 512;
std::mutex mutex;
typedef std::vector<char> data_type;

void thread1()
{
  bar1.wait();
  int tem = 0;
  for (size_t i=0; i < COUNT; ++i)
  {
    std::lock_guard<std::mutex> lk(mutex);
    ++tem;
    //std::cout << i << std::endl;
  }
  std::cout << std::endl <<  tem << std::endl;
}

void thread2()
{
  bar2.wait();
  int tem = 0;
  for (size_t i=0; i < COUNT; ++i)
  {
    ++tem;
    auto d = std::unique_ptr<data_type>( new data_type(SIZE) );
    d.reset();
    //std::cout << i << std::endl;
  }
  std::cout << std::endl <<  tem << std::endl;
}


UNIT(factory, "")
{
  using namespace fas::testing;

  std::thread t3(thread2);
  std::thread t4(thread2);
  auto start2 = std::chrono::high_resolution_clock::now();
  bar2.wait();
  t3.join();
  t4.join();
  auto finish2 = std::chrono::high_resolution_clock::now();
  auto ms2 = std::chrono::duration_cast<std::chrono::microseconds>( finish2 - start2 ).count();
  t << message("threads2: ") <<  ms2 << "ms";
  t << nothing;

  
  std::thread t1(thread1);
  std::thread t2(thread1);
  auto start = std::chrono::high_resolution_clock::now();
  bar1.wait();
  t1.join();
  t2.join();
  auto finish = std::chrono::high_resolution_clock::now();
  auto ms1 = std::chrono::duration_cast<std::chrono::microseconds>( finish - start ).count();
  t << message("threads1: ") <<  ms1 << "ms";
  t << nothing;


}

BEGIN_SUITE(factory,"")
  ADD_UNIT(factory)
END_SUITE(factory)

