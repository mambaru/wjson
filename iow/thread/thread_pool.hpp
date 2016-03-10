#pragma once

#include <iow/thread/delayed_queue.hpp>

#include <thread>
#include <mutex>
#include <vector>

namespace iow {

struct thread_pool_options
{
  size_t threads = 0;
};

template<typename Service = delayed_queue>
class thread_pool
{
public:

  typedef Service service_type;
  typedef std::shared_ptr<service_type> service_ptr;
  
  // service_ptr допустим nullptr
  thread_pool(service_ptr service)
    : _service(service)
  {}
    
  template<typename Opt>
  void start(Opt opt)
  {
    std::lock_guard< std::mutex > lk(_mutex);
    if ( _service==nullptr )
      return;
    
    _threads.reserve(opt.threads);
    for (int i = 0 ; i < opt.threads; ++i)
    {
      _threads.push_back( std::thread( [this]()
      {
        this->_service.run();
      }));
    }
  }

  // только после _service->stop();
  void stop()
  {
    std::lock_guard< std::mutex > lk(_mutex);
    if ( _service==nullptr )
      return;

    for (auto& t : _threads)
      t.join();
  }
  
private:
  std::mutex _mutex;
  service_ptr _service;
  std::vector< std::thread > _threads;
};

}
