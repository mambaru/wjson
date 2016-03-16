#pragma once 

#include <iow/workflow/queue_options.hpp>

#include <chrono>
#include <queue>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

namespace iow {

class delayed_queue
{
  struct queue_cmp;

public:

  typedef std::function<void()>                               function_t;
  typedef std::chrono::time_point<std::chrono::system_clock>  time_point_t;
  typedef time_point_t::duration                              duration_t;
  typedef std::pair<time_point_t, function_t>                 event_t;
  typedef std::condition_variable                             condition_variable_t;
  typedef std::mutex                                          mutex_t;
  typedef std::queue<function_t>                              queue_t;
  typedef std::priority_queue<event_t, std::vector<event_t>, queue_cmp>  delayed_queue_t;

  delayed_queue( delayed_queue const & ) = delete;
  void operator=( delayed_queue const & ) = delete;

  delayed_queue(const queue_options& opt)
    : _opt(opt)
    , _loop_exit( true )
  {}
  
  virtual ~delayed_queue ()
  {
    this->stop();
  }
  
  void reconfigure(const queue_options& opt)
  {
    std::unique_lock<mutex_t> lck( _mutex );
    _opt = opt;
  }
  
  void run()
  {
    std::unique_lock<mutex_t> lck( _mutex );
    if ( _loop_exit )
      _loop_exit = false;
    lck.unlock();
    this->loop_(lck, false);
  }
  
  bool run_one()
  {
    std::unique_lock<mutex_t> lck( _mutex );
    if ( _loop_exit )
      _loop_exit = false;
    lck.unlock();
    return this->run_one_( lck );
  }
  
  bool poll_one()
  {
    std::unique_lock<mutex_t> lck( _mutex, std::defer_lock );
    return this->poll_one_( lck );
  }
  
  void stop()
  {
    std::unique_lock<mutex_t> lck( _mutex );
    if ( ! _loop_exit )
    { 
      _loop_exit = true;
      _cond_var.notify_all();
    }
  }
  
  template<typename F>
  bool post( F f )
  {
    std::lock_guard<mutex_t> lock( _mutex );
    _que.push( std::move( f ) );
    _cond_var.notify_one();
    return true;
  }
  
  template<typename TP, typename F>
  bool post_at(TP time_point, F f)
  {
    std::lock_guard<mutex_t> lock( _mutex );
    this->push_at_( std::move(time_point), std::move(f) ); 
    _cond_var.notify_one();
    return true;
  }

  template<typename D, typename F>
  bool delayed_post(D duration, F f)
  {  
    std::lock_guard<mutex_t> lock( _mutex );
    if ( ! duration.count() )
      _que.push( std::move( f ) );
    else
      this->push_at_( std::move( std::chrono::system_clock::now() + duration), std::move(f)  );
    _cond_var.notify_one();
    return true;
  }
  
  std::size_t size() const
  {
    std::lock_guard<mutex_t> lck( _mutex );
    return _que.size() + _delayed_que.size();
  }

private:

  template<typename TP, typename F>
  void push_at_(TP time_point, F f)
  {
    _delayed_que.emplace( time_point, std::move( f ) );
  }
  
  bool poll_one_( std::unique_lock<mutex_t>& lck)
  {
    function_t run_func;

    lck.lock();
    if ( ! _delayed_que.empty() )
    {
      if ( _delayed_que.top().first <= std::chrono::system_clock::now() )
      {
        _que.push( std::move( _delayed_que.top().second ) );
        _delayed_que.pop();
      }
    }
    if ( _que.empty() )
    {
      lck.unlock();
      return false;
    }
    run_func.swap( _que.front() );
    _que.pop();
    lck.unlock();
    
    run_func();
    
    return true;
  }

  bool run_one_( std::unique_lock<mutex_t>& lck)
  {
    return this->loop_( lck, true);
  }

  bool loop_(std::unique_lock<mutex_t>& lck, bool one)
  {
    while ( !_loop_exit )
    {
      if ( !this->poll_one_( lck ) )
      {
        this->run_wait_(lck);
      } 
      else if ( one )
      {
        return true;
      }
    }
    return false;
  }

  void run_wait_( std::unique_lock<mutex_t> & lck)
  {
    lck.lock();
    if ( _que.empty() && _delayed_que.empty() )
    {
      _cond_var.wait( lck );
    }
    else if ( _que.empty() && !_delayed_que.empty() )
    {
      _cond_var.wait_until( lck, _delayed_que.top().first );
    }
    lck.unlock();
  }

  struct queue_cmp
  {
    inline bool operator()( delayed_queue::event_t & e1, delayed_queue::event_t & e2 )
    {
      return e1.first > e2.first;
    }
  };

//members

  queue_options            _opt;
  mutable mutex_t          _mutex;
  condition_variable_t     _cond_var;
  queue_t                  _que;
  delayed_queue_t          _delayed_que;
  bool                     _loop_exit;

}; // delayed_queue
}


