
#include "delayed_queue.hpp"

namespace iow {

delayed_queue::delayed_queue()
  : loop_exit_( true )
{
}

delayed_queue::~delayed_queue()
{
  this->stop();
}

void
delayed_queue::run()
{
  std::unique_lock<mutex_t> lck( mutex_ );
  if ( loop_exit_ )
    loop_exit_ = false;
  lck.unlock();
  this->loop();
}

bool
delayed_queue::run_one()
{
  function_t run_func;
  std::unique_lock<mutex_t> lck( mutex_, std::defer_lock );
  return this->run_one( run_func, lck );
}

void
delayed_queue::stop()
{
  if ( ! loop_exit_ )
  { 
    loop_exit_ = true;
    cond_var_.notify_all();
  }
}

void
delayed_queue::post( function_t && f )
{
  std::lock_guard<mutex_t> lock( mutex_ );
  que_.push( std::move( f ) );
  cond_var_.notify_one();
}


void
delayed_queue::delayed_post( duration_t && delay, function_t && f )
{
  std::lock_guard<mutex_t> lock( mutex_ );
  if ( ! delay.count() )
    que_.push( std::move( f ) );
  else
    delayed_que_.emplace( std::chrono::steady_clock::now() + delay, std::move( f ) );
  cond_var_.notify_one();
}

std::size_t
delayed_queue::size() const
{
  std::lock_guard<mutex_t> lck( mutex_ );
  return que_.size();
}

std::size_t
delayed_queue::waits() const
{
  std::lock_guard<mutex_t> lck( mutex_ );
  return delayed_que_.size();
}

bool
delayed_queue::run_one( function_t & run_func, std::unique_lock<mutex_t> & lck)
{
  lck.lock();
  if ( ! delayed_que_.empty() )
  {
    if ( delayed_que_.top().first <= std::chrono::steady_clock::now() )
    {
      que_.push( std::move( delayed_que_.top().second ) );
      delayed_que_.pop();
    }
  }
  if ( que_.empty() )
  {
    lck.unlock();
    return false;
  }
  run_func.swap( que_.front() );
  que_.pop();
  lck.unlock();
  /*
  try
  {
    run_func();
  }
  catch( std::exception & e ) { std::cerr << "dque exception: " << e.what() << "\n"; }
  catch( ... )                { std::cerr << "dque unknown exception\n"; }
  */
  run_func();
  return true;
}

void
delayed_queue::loop()
{
  function_t run_func;
  std::unique_lock<mutex_t> lck( mutex_, std::defer_lock );
  while ( !loop_exit_ )
  {
    if ( !this->run_one( run_func, lck ) )
    {
      lck.lock();
      if ( que_.empty() && delayed_que_.empty() )
        cond_var_.wait( lck );
      else if ( que_.empty() && !delayed_que_.empty() )
        cond_var_.wait_until( lck, delayed_que_.top().first );
      lck.unlock();
    }
  }
}

} // ns util

