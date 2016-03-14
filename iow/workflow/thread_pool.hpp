#pragma once

#include <iow/workflow/delayed_queue.hpp>

#include <thread>
#include <mutex>
#include <vector>

namespace iow {

/*struct thread_pool_options
{
  size_t threads = 0;
};
*/

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
  
  bool reconfigure(int threads)
  {
    if ( threads < _threads.size() ) 
      return false;
    
    int diff = threads - _threads.size();
    this->run_more_(diff);
    return true;
  }
    
  void start(int threads)
  {
    std::lock_guard< std::mutex > lk(_mutex);
    if ( _service==nullptr || !_threads.empty())
      return;
    
    this->run_more_(threads);
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
  
  void run_more_(int threads)
  {
    _threads.reserve( _threads.size() + threads);
    for (int i = 0 ; i < threads; ++i)
    {
      _threads.push_back( std::thread( [this]()
      {
        this->_service->run();
      }));
    }
  }
  
private:
  std::mutex _mutex;
  service_ptr _service;
  std::vector< std::thread > _threads;
};

}
