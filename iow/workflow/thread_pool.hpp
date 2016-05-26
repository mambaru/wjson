#pragma once

#include <iow/workflow/delayed_queue.hpp>

#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

namespace iow {

template<typename Service = delayed_queue>
class thread_pool
  : public std::enable_shared_from_this< thread_pool<Service> >
{
public:
  typedef thread_pool<Service> self;
  typedef std::shared_ptr<bool> thread_flag;
  typedef Service service_type;
  typedef std::shared_ptr<service_type> service_ptr;
  typedef std::vector<thread_flag> flag_list;
  
  // service_ptr допустим nullptr
  thread_pool(service_ptr service)
    : _started(false)
    , _service(service)
  {
    _rate_limit = 0;
  }
  
  void rate_limit(size_t rps) 
  {
    _rate_limit = rps;
  }

  bool reconfigure(int threads)
  {
    std::lock_guard< std::mutex > lk(_mutex);
    
    if ( !_started )
      return false;
    
    if ( threads == _threads.size() ) 
      return false;
    
    if ( threads > _threads.size() ) 
    {
      int diff = threads - _threads.size();
      this->run_more_(diff);
    }
    else
    {
      for ( size_t i = threads; i < _threads.size(); ++i)
        _threads[i].detach();
      _threads.resize(threads);
      _flags.resize(threads);
    }
    return true;
  }
    
  void start(int threads)
  {
    std::lock_guard< std::mutex > lk(_mutex);
    
    if ( _started )
      return;
    
    _started = true;
    
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
    
    _threads.clear();
    
    _started = false;
  }
  
private: 
  
  void run_more_(int threads)
  {
    _threads.reserve( _threads.size() + threads);
    for (int i = 0 ; i < threads; ++i)
    {
      thread_flag pflag = std::make_shared<bool>(true);
      std::weak_ptr<bool> wflag = pflag;
      std::weak_ptr<self> wthis = this->shared_from_this();
      _flags.push_back(pflag);
      _threads.push_back( std::thread( [wthis, wflag]()
      {
        auto start = std::chrono::system_clock::now();
        size_t count = 0;
        while ( auto pthis = wthis.lock() )
        {
          if ( !pthis->_service->run_one() )
            break;
          if ( wflag.lock() == nullptr)
            break;
          
          if ( pthis->_rate_limit != 0 )
          {
            ++count;
            if ( count > pthis->_rate_limit )
            {
              auto now = std::chrono::system_clock::now();
              auto tm_ms = std::chrono::duration_cast< std::chrono::milliseconds >( now - start ).count();
              if ( tm_ms < 1000 )
                std::this_thread::sleep_for( std::chrono::milliseconds(1000-tm_ms)  );
              
              count = 0;
              start = std::chrono::system_clock::now();
            }
          }
        }
      }));
    }
  }
  
private:
  bool _started;
  std::atomic<size_t> _rate_limit;
  std::mutex _mutex;
  service_ptr _service;
  std::vector< std::thread > _threads;
  std::vector< thread_flag > _flags;
};

}
