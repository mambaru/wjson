#pragma once

#include <iow/asio.hpp>
#include <iow/workflow/delayed_queue.hpp>
#include <iow/workflow/timed_queue.hpp>
#include <iow/workflow/queue_options.hpp>

namespace iow{
 
class bique
{
  typedef std::shared_ptr<delayed_queue> delayed_ptr;
  typedef std::shared_ptr<timed_queue> timed_ptr;
public:
  typedef ::iow::asio::io_service io_service_type;
  typedef delayed_queue::function_t function_t;
  typedef delayed_queue::time_point_t time_point_t;
  typedef delayed_queue::duration_t duration_t;
  
  virtual ~bique()
  {
    this->stop();
  }
 
  bique( const queue_options& opt )
  {
    _dflag = true;
    _delayed = std::make_shared<delayed_queue>(opt);
    _timed  = nullptr;
  }

  bique( io_service_type& io, const queue_options& opt, bool timed /*= true*/ )
  {
    _dflag = !timed;
    _delayed = std::make_shared<delayed_queue>(opt);
    _timed  = std::make_shared<timed_queue>(io, opt);
  }
  
  void reconfigure(const queue_options& opt, bool timed /*= true*/)
  {
    bool newflag = !timed;
    if ( _timed == nullptr ) newflag = true;
    _dflag = newflag;
    _delayed->reconfigure(opt);
    if( _timed ) _timed->reconfigure(opt);
  }

  void reset()
  {
    return this->invoke_( &delayed_queue::reset, &timed_queue::reset);
  }

  bool run()
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

  bool post( function_t f )
  {
    return this->invoke_( &delayed_queue::post, &timed_queue::post, std::move(f) );
  }

  bool post_at(time_point_t tp, function_t&& f)
  {
    return this->invoke_( &delayed_queue::post_at, &timed_queue::post_at, tp, std::move(f));
  }

  bool delayed_post(duration_t duration, function_t&& f)
  {
    return this->invoke_( &delayed_queue::delayed_post, &timed_queue::delayed_post, duration, std::move(f));
  }
  
  std::size_t size() const
  {
    return this->invoke_( &delayed_queue::size, &timed_queue::size);
  }
  
private:
  
  template<typename R, typename... Args>
  R invoke_( 
    R(delayed_queue::* method1)(Args...),
    R(timed_queue::* method2)(Args...), 
    Args... args)
  {
    return _dflag 
      ? (_delayed.get()->*method1)( std::move(args)...)
      : (_timed.get()->*method2)( std::move(args)...);
  }

  template<typename R, typename... Args>
  R invoke_( 
    R(delayed_queue::* method1)(Args...) const,
    R(timed_queue::* method2)(Args...) const, 
    Args... args) const
  {
    return _dflag 
      ? (_delayed.get()->*method1)( std::move(args)...)
      : (_timed.get()->*method2)( std::move(args)...);
  }

private:
  std::atomic<bool> _dflag;
  delayed_ptr _delayed;
  timed_ptr   _timed;
};

}
