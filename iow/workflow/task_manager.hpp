#pragma once

#include <iow/asio.hpp>
#include <iow/workflow/timer_manager.hpp>
#include <iow/workflow/bique.hpp>
#include <iow/workflow/queue_options.hpp>
#include <iow/workflow/thread_pool.hpp>

namespace iow{
 
class task_manager
{
public:
  typedef bique queue_type;

  typedef std::function<void()>                               function_t;
  typedef std::chrono::time_point<std::chrono::system_clock>  time_point_t;
  typedef time_point_t::duration                              duration_t;
  typedef ::iow::asio::io_service io_service_type;

  typedef timer_manager<queue_type> timer_type;
  typedef thread_pool<queue_type> pool_type;

  task_manager( io_service_type& io, const queue_options& opt )
    : _threads(0)
  {
    // delayed отключен и пул потоков
    _queue = std::make_shared<queue_type>(io, opt, true );
    _timer = std::make_shared<timer_type>(_queue);
    _pool = nullptr;
  }
  
  task_manager( const queue_options& opt, int threads )
    : _threads(threads)
  {
    _queue = std::make_shared<queue_type>(opt);
    _timer = std::make_shared<timer_manager<queue_type> >(_queue);
    _pool = std::make_shared<pool_type>(_queue);
  }
  
  task_manager( io_service_type& io, const queue_options& opt, int threads, bool use_asio /*= false*/  )
    : _threads(threads)
  {
    _queue = std::make_shared<queue_type>(io, opt, threads==0 || use_asio );
    _timer = std::make_shared<timer_type>(_queue);
    _pool = std::make_shared<pool_type>(_queue);
  }

  void reconfigure(const queue_options& opt, int threads, bool use_asio /*= false*/ )
  {
    _threads = threads;
    _queue->reconfigure(opt, threads==0 || use_asio );
  }
  
  void rate_limit(size_t rps) 
  {
    if ( _pool!=nullptr) 
      _pool->rate_limit(rps);
  }
  

  void start()
  {
    if ( _pool!=nullptr) 
      _pool->start(_threads);
  }

  void stop()
  {
    _timer->clear();
    _queue->stop();
    if ( _pool!=nullptr) 
      _pool->stop();
    _queue->reset();
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
  
  
  bool post( function_t&& f )
  {
    return _queue->post(std::move(f) );
  }
  
  bool post_at(time_point_t tp, function_t&& f)
  {
    return _queue->post_at( tp, std::move(f));
  }

  bool delayed_post(duration_t duration, function_t&& f)
  {
    return _queue->delayed_post(duration, std::move(f));
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
  std::shared_ptr<queue_type> _queue;
  std::shared_ptr<timer_type> _timer;
  std::shared_ptr<pool_type>  _pool;
};

}
