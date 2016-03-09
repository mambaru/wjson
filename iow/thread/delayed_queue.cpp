
#include "delayed_queue.hpp"

namespace iow {


bool delayed_queue::run_one()
{
  std::unique_lock<mutex_t> lck( _mutex );
  if ( _loop_exit )
    _loop_exit = false;
  lck.unlock();
  return this->run_one_( lck );
}

bool delayed_queue::poll_one()
{
  function_t run_func;
  std::unique_lock<mutex_t> lck( _mutex, std::defer_lock );
  return this->poll_one_( run_func, lck );
}

void delayed_queue::stop()
{
  std::unique_lock<mutex_t> lck( _mutex );
  if ( ! _loop_exit )
  { 
    _loop_exit = true;
    _cond_var.notify_all();
  }
}

void delayed_queue::post( function_t && f )
{
  std::lock_guard<mutex_t> lock( _mutex );
  _que.push( std::move( f ) );
  _cond_var.notify_one();
}


void delayed_queue::delayed_post( duration_t && delay, function_t && f )
{
  std::lock_guard<mutex_t> lock( _mutex );
  if ( ! delay.count() )
    _que.push( std::move( f ) );
  else
    _delayed_que.emplace( std::chrono::steady_clock::now() + delay, std::move( f ) );
  _cond_var.notify_one();
}

std::size_t delayed_queue::size() const
{
  std::lock_guard<mutex_t> lck( _mutex );
  return _que.size();
}

std::size_t delayed_queue::waits() const
{
  std::lock_guard<mutex_t> lck( _mutex );
  return _delayed_que.size();
}


} // ns util

