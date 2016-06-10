#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <memory>

namespace iow {

class bique;
class asio_queue;
class delayed_queue;

class thread_pool_base
  : public std::enable_shared_from_this< thread_pool_base >
{
public:
  typedef thread_pool_base self;
  typedef std::shared_ptr<bool> thread_flag;
  typedef std::vector<thread_flag> flag_list;
  
  thread_pool_base();
  
  void rate_limit(size_t rps);

  
  bool reconfigure(std::shared_ptr<bique> s, int threads);
  bool reconfigure(std::shared_ptr<asio_queue> s, int threads);
  bool reconfigure(std::shared_ptr<delayed_queue> s, int threads);

  void start(std::shared_ptr<bique> s, int threads);
  void start(std::shared_ptr<asio_queue> s, int threads);
  void start(std::shared_ptr<delayed_queue> s, int threads);

  // только после _service->stop();
  void stop();
  
private: 
  template<typename S>
  bool reconfigure_(std::shared_ptr<S> s, int threads);

  template<typename S>
  void start_(std::shared_ptr<S> s, int threads);

  template<typename S>
  void run_more_(std::shared_ptr<S> s, int threads);

private:
  bool _started;
  std::atomic<size_t> _rate_limit;
  std::mutex _mutex;
  std::vector< std::thread > _threads;
  std::vector< thread_flag > _flags;
};

}
