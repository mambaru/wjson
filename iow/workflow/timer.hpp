#pragma once

#include <iow/memory.hpp>
#include <chrono>

namespace iow{

class delayed_queue;
class asio_queue;
class bique;

class timer
{
public:
  typedef std::weak_ptr<bool> wflag_type;

  typedef std::function<bool()> handler;
  typedef std::function<void(bool)> handler_callback;
  typedef std::function<void(handler_callback)> async_handler;

  typedef std::chrono::system_clock         clock_t;
  typedef std::chrono::time_point<clock_t>  time_point_t;
  typedef time_point_t::duration            duration_t;

  static std::function<void()> make( std::shared_ptr<bique>         pq, duration_t delay,  handler h, bool expires_after, wflag_type wflag );
  static std::function<void()> make( std::shared_ptr<asio_queue>    pq, duration_t delay,  handler h, bool expires_after, wflag_type wflag );
  static std::function<void()> make( std::shared_ptr<delayed_queue> pq, duration_t delay, handler h, bool expires_after, wflag_type wflag );

  static std::function<void()> make( std::shared_ptr<bique> pq,        duration_t delay,  async_handler h, bool expires_after, wflag_type wflag );
  static std::function<void()> make( std::shared_ptr<asio_queue> pq,   duration_t delay,  async_handler h, bool expires_after, wflag_type wflag );
  static std::function<void()> make( std::shared_ptr<delayed_queue> pq, duration_t delay, async_handler h, bool expires_after, wflag_type wflag );

  static bool today_from_string(const std::string& strtime, time_point_t& result);
};

}
