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
  struct QueCmp;

public:

  typedef std::function<void()>                                       function_t;
  typedef std::chrono::time_point<std::chrono::steady_clock>          time_point_t;
  typedef time_point_t::duration                                      duration_t;
  typedef std::pair<time_point_t, function_t>                         event_t;
  typedef std::condition_variable                                     condition_variable_t;
  typedef std::mutex                                                  mutex_t;
  typedef std::queue<function_t>                                      que_t;
  typedef std::priority_queue<event_t, std::vector<event_t>, QueCmp>  delayed_que_t;

              delayed_queue  ();
              ~delayed_queue ();
              delayed_queue  ( delayed_queue const & ) = delete;
  void        operator=     ( delayed_queue const & ) = delete;
  void        run           ();
  bool        run_one       ();
  void        stop          ();
  void        post          ( function_t && );
  void        delayed_post  ( duration_t &&, function_t && );
  std::size_t size          () const;
  std::size_t waits         () const;

private:

  bool run_one( function_t &, std::unique_lock<mutex_t> & );
  void loop();

  struct QueCmp
  {
    inline
    bool operator()( delayed_queue::event_t & e1, delayed_queue::event_t & e2 )
    {
      return e1.first > e2.first;
    }
  };

//members

  mutex_t mutable          mutex_;
  condition_variable_t     cond_var_;
  que_t                    que_;
  delayed_que_t            delayed_que_;
  bool                     loop_exit_;

}; // delayed_queue

} // ns util

#endif

