#pragma once

#include <iow/asio.hpp>
#include <iow/thread/timer_manager.hpp>
#include <iow/thread/delayed_queue.hpp>
#include <iow/thread/timed_queue.hpp>
#include <iow/thread/queue_options.hpp>
#include <iow/thread/thread_pool.hpp>

namespace iow{
 
struct work_options: queue_options
{
  int threads = 0;
};

class work_queue
{
  typedef std::shared_ptr<delayed_queue> delayed_ptr;
  typedef std::shared_ptr<timed_queue> timed_ptr;
public:
  typedef ::iow::asio::io_service io_service_type;
  
  work_queue( const work_options& opt )
  {
    _dflag = true;
    _delayed = std::make_shared<delayed_queue>(opt);
    _timed  = nullptr;
  }

  work_queue( io_service_type& io, const work_options& opt )
  {
    _dflag = ( opt.threads!=0 );
    _delayed = std::make_shared<delayed_queue>(opt);
    _timed  = std::make_shared<timed_queue>(io, opt);
  }
  
  void reconfigure(const work_options& opt)
  {
    bool newflag = (opt.threads!=0);
    if ( _timed == nullptr ) newflag = true;
    _dflag = newflag;
    _delayed->reconfigure(opt);
    _timed->reconfigure(opt);
  }
  
  void run()
  {
    return this->invoke_( &delayed_queue::run, &timed_queue::run);
  }
  
  bool run_one()
  {
    return this->invoke_( &delayed_queue::run_one, &timed_queue::run_one);
  }
  
  bool poll_one()
  {
    return this->invoke_( &delayed_queue::poll_one, &timed_queue::poll_one);
  }
  
  void stop()
  {
    return this->invoke_( &delayed_queue::stop, &timed_queue::stop);
  }
  
  template<typename F>
  bool post( F f )
  {
    return this->invoke_( &delayed_queue::post<F>, &timed_queue::post<F>, std::forward<F>(f) );
  }
  
  template<typename TP, typename F>
  bool post_at(TP tp, F f)
  {
    return this->invoke_( &delayed_queue::post_at<TP, F>, &timed_queue::post_at<TP, F>, std::forward<TP>(tp), std::forward<F>(f));
  }

  template<typename D, typename F>
  bool delayed_post(D duration, F f)
  {
    return this->invoke_( &delayed_queue::delayed_post<D, F>, &timed_queue::delayed_post<D, F>, std::forward<D>(duration), std::forward<F>(f));
  }
  
  std::size_t size() const
  {
    return _dflag ? _delayed->size() : _timed->size();
  }
  
private:
  
  template<typename R, typename... Args>
  R invoke_( 
    R(delayed_queue::* method1)(Args...),
    R(timed_queue::* method2)(Args...), 
    Args... args)
  {
    if ( _dflag )
      return (_delayed.get()->*method1)( std::forward<Args>(args)...);
    else
      return (_timed.get()->*method2)( std::forward<Args>(args)...);
  }
  
private:
  std::atomic<bool> _dflag;
  delayed_ptr _delayed;
  timed_ptr   _timed;
};

struct dataflow_options: work_options
{
  
};

class dataflow
{
public:
  
  typedef ::iow::asio::io_service io_service_type;
  typedef timer_manager<work_queue> timer_type;
  typedef thread_pool<work_queue> pool_type;
  
  dataflow( io_service_type& io, const dataflow_options& opt )
    : _threads(opt.threads)
  {
    _queue = std::make_shared<work_queue>(io, opt);
    _timer = std::make_shared<timer_manager<work_queue> >(_queue);
    _pool = std::make_shared<pool_type>(_queue);
  }
  
  void reconfigure(const dataflow_options& opt)
  {
    _queue->reconfigure(opt);
    if ( !_pool->reconfigure(_threads) )
    {
      // Если уменьшаем количество потоков, нужно все остановить, чтобы освободить уже запущенные потоки
      _queue->stop();
      _pool->stop();
      _threads = opt.threads;
      _timer = std::make_shared<timer_manager<work_queue> >(_queue);
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
  std::shared_ptr<work_queue> _queue;
  std::shared_ptr<timer_type> _timer;
  std::shared_ptr<pool_type>  _pool;
};

}
