#ifndef MAMBA_DELAYED_QUEUE_HPP
#define MAMBA_DELAYED_QUEUE_HPP

//#include <cstddef>

#include <chrono>
#include <queue>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace iow {

class delayed_queue
{
  struct queue_cmp;

public:

  typedef std::function<void()>                                       function_t;
  typedef std::chrono::time_point<std::chrono::steady_clock>          time_point_t;
  typedef time_point_t::duration                                      duration_t;
  typedef std::pair<time_point_t, function_t>                         event_t;
  typedef std::condition_variable                                     condition_variable_t;
  typedef std::mutex                                                  mutex_t;
  typedef std::queue<function_t>                                      queue_t;
  typedef std::priority_queue<event_t, std::vector<event_t>, queue_cmp>  delayed_queue_t;

  delayed_queue  ();
  ~delayed_queue ();
  delayed_queue( delayed_queue const & ) = delete;
  void operator=( delayed_queue const & ) = delete;
  
  void run();
  bool run_one();
  bool poll_one();
  void stop();
  void post( function_t && );
  void delayed_post(duration_t &&, function_t &&);
  std::size_t size() const;
  std::size_t waits() const;

private:

  bool run_one_( function_t&, std::unique_lock<mutex_t> & );
  bool poll_one_( function_t&, std::unique_lock<mutex_t> & );
  void loop();

  struct queue_cmp
  {
    inline bool operator()( delayed_queue::event_t & e1, delayed_queue::event_t & e2 )
    {
      return e1.first > e2.first;
    }
  };

//members

  mutable mutex_t          _mutex;
  condition_variable_t     cond_var_;
  queue_t                  que_;
  delayed_queue_t          delayed_que_;
  bool                     loop_exit_;

}; // delayed_queue
}

#include <iow/asio.hpp>

namespace iow{
  
class delayed_io_post
{
public:
  typedef ::iow::asio::deadline_timer timer_type;
  typedef ::iow::asio::io_service io_service;
public:
  delayed_io_post( delayed_io_post const & ) = delete;
  void operator=( delayed_io_post const & ) = delete;

  delayed_io_post(io_service& io)
    : _timer(io)
  {}
  
  timer_type& timer() { return _timer;}
  const timer_type& timer() const { return _timer;}
  
  void run()
  {
    _timer.get_io_service().run();
  }
  
  bool run_one()
  {
    return 0!=_timer.get_io_service().run_one();
    
  }
  
  bool poll_one()
  {
    return 0!=_timer.get_io_service().poll_one();
  }
  
  void stop()
  {
    _timer.get_io_service().stop();
  }
  
  
  template<typename F>
  void post( F f )
  {
    _timer.get_io_service().post( std::move(f) );
  }
  
  template<typename D, typename F>
  void delayed_post(D d, F f)
  {
    using namespace std::chrono;
    _timer.expires_from_now( ::boost::posix_time::milliseconds( duration_cast<milliseconds>(d).count() ), std::move(f) );
  }
  
  std::size_t size() const
  {
    return 0;
  }
  
  std::size_t waits() const
  {
    return 0;
  }

  
private:
  timer_type _timer;
};

} // ns util

#endif

