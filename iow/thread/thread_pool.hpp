#pragma once

#include <iow/thread/delayed_queue.hpp>

#include <thread>
#include <mutex>
#include <vector>

namespace iow {

struct thread_pool_options
{
  
};

template<typename Queue = delayed_queue>
class thread_pool
{
public:

  typedef Queue queue_type;  
  
  ~thread_pool()
  {
    this->stop();
  }
  
  template<typename Handler>
  void post( Handler h )
  {
    _queue.post( std::move(h) );
  }
  
  template<typename Duration, typename Handler>
  void delayed_post(Duration d, Handler h)
  {
    _queue.delayed_post( std::move(h) );
  }
  
  size_t size() const 
  {
    return _queue.size();
  }

  size_t wait() const 
  {
    return _queue.size();
  }

  template<typename Opt>
  void start(Opt opt)
  {
    this->stop();
    
    std::lock_guard< std::mutex > lk(_mutex);
    _threads.reserve(opt.threads);
    for (int i = 0 ; i < opt.threads; ++i)
    {
      _threads.push_back( std::thread( [this]()
      {
        this->_queue.run();
      }) );
    }
  }
  
  void stop()
  {
    _queue->stop();
    
    std::lock_guard< std::mutex > lk(_mutex);
    for (auto& t : _threads)
      t.join();
  }
  
private:
  std::mutex _mutex;
  queue_type _queue;
  std::vector< std::thread > _threads;
};

}



