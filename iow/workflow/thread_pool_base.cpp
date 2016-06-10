
#include "thread_pool_base.hpp"
#include <iow/workflow/delayed_queue.hpp>
#include <iow/workflow/bique.hpp>
#include <iow/workflow/asio_queue.hpp>

namespace iow {

thread_pool_base::thread_pool_base()
  : _started(false)
{
  _rate_limit = 0;
}

void thread_pool_base::rate_limit(size_t rps) 
{
  _rate_limit = rps;
}

bool thread_pool_base::reconfigure(std::shared_ptr<bique> s, int threads) 
{
  return this->reconfigure_(s, threads); 
}

bool thread_pool_base::reconfigure(std::shared_ptr<asio_queue> s, int threads)
{
  return this->reconfigure_(s, threads); 
}

bool thread_pool_base::reconfigure(std::shared_ptr<delayed_queue> s, int threads) 
{
  return this->reconfigure_(s, threads); 
}

void thread_pool_base::start(std::shared_ptr<bique> s, int threads) 
{
  this->start_(s, threads); 
}

void thread_pool_base::start(std::shared_ptr<asio_queue> s, int threads)
{ 
  this->start_(s, threads); 
}

void thread_pool_base::start(std::shared_ptr<delayed_queue> s, int threads) 
{
  this->start_(s, threads); 
}


// только после _service->stop();
void thread_pool_base::stop()
{
  std::lock_guard< std::mutex > lk(_mutex);

  for (auto& t : _threads)
    t.join();

  _threads.clear();

  _started = false;
}

template<typename S>
bool thread_pool_base::reconfigure_(std::shared_ptr<S> s, int threads)
{
  std::lock_guard< std::mutex > lk(_mutex);
  
  if ( !_started )
    return false;
  
  if ( threads == _threads.size() ) 
    return false;
  
  if ( threads > _threads.size() ) 
  {
    int diff = threads - _threads.size();
    this->run_more_(s, diff);
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

template<typename S>
void thread_pool_base::start_(std::shared_ptr<S> s, int threads)
{
  std::lock_guard< std::mutex > lk(_mutex);
  
  if ( _started )
    return;
  
  _started = true;
  
  if ( !_threads.empty())
    return;
  
  this->run_more_(s, threads);
}

template<typename S>
void thread_pool_base::run_more_(std::shared_ptr<S> s, int threads)
{
  _threads.reserve( _threads.size() + threads);
  for (int i = 0 ; i < threads; ++i)
  {
    thread_flag pflag = std::make_shared<bool>(true);
    std::weak_ptr<bool> wflag = pflag;
    std::weak_ptr<self> wthis = this->shared_from_this();
    _flags.push_back(pflag);
    _threads.push_back( std::thread( [wthis, s, wflag]()
    {
      auto start = std::chrono::system_clock::now();
      size_t count = 0;
      while ( auto pthis = wthis.lock() )
      {
        if ( !s->run_one() )
          break;
        if ( wflag.lock() == nullptr)
          break;
        
        if ( pthis->_rate_limit != 0 )
        {
          ++count;
          if ( count >= pthis->_rate_limit )
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

}
