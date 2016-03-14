#pragma once

#include <iow/asio.hpp>
#include <iow/workflow/timer_manager.hpp>
#include <iow/workflow/bique.hpp>
#include <iow/workflow/queue_options.hpp>
#include <iow/workflow/thread_pool.hpp>

namespace iow{
 
class workflow
{
public:
  
  typedef ::iow::asio::io_service io_service_type;
  typedef timer_manager<bique> timer_type;
  typedef thread_pool<bique> pool_type;
  
  workflow( io_service_type& io, const queue_options& opt, int threads = 0 )
    : _threads(threads)
  {
    _queue = std::make_shared<bique>(io, opt, threads==0 );
    _timer = std::make_shared<timer_manager<bique> >(_queue);
    _pool = std::make_shared<pool_type>(_queue);
  }
  
  void reconfigure(const queue_options& opt, int threads = 0)
  {
    _queue->reconfigure(opt);
    _threads = threads;
    if ( !_pool->reconfigure(_threads) )
    {
      // Если уменьшаем количество потоков, нужно все остановить, чтобы освободить уже запущенные потоки
      _queue->stop();
      _pool->stop();
      _timer = std::make_shared<timer_manager<bique> >( _queue );
      _pool = std::make_shared<pool_type>(_queue);
      _pool->start(_threads);
    }
  }

  void start()
  {
    _pool->start(_threads);
  }

  void stop()
  {
    _timer->clear();
    _queue->stop();
    _pool->stop();
  }

  void run()
  {
    _queue->run();
  }
  
  bool run_one()
  {
    return _queue->run_one();
  }
  
  bool poll_one()
  {
    return _queue->poll_one();
  }
  
  template<typename F>
  bool post( F f )
  {
    return _queue->post(std::forward<F>(f) );
  }
  
  template<typename TP, typename F>
  bool post_at(TP tp, F f)
  {
    return _queue->post_at( std::forward<TP>(tp), std::forward<F>(f));
  }

  template<typename D, typename F>
  bool delayed_post(D duration, F f)
  {
    return _queue->delayed_post( std::forward<D>(duration), std::forward<F>(f));
  }
  
  std::size_t size() const
  {
    return _queue->size();
  }
  
  std::shared_ptr<timer_type> timer()
  {
    return _timer;
  }

private:
  std::atomic<int> _threads;
  std::shared_ptr<bique> _queue;
  std::shared_ptr<timer_type> _timer;
  std::shared_ptr<pool_type>  _pool;
};

}
