#pragma once

#include <iow/memory.hpp>
#include <iow/asio.hpp>
#include <iow/system.hpp>
#include <iow/logger/logger.hpp>
#include <iow/owner/owner.hpp>

#include <iow/workflow/timer.hpp>
#include <iow/workflow/requester.hpp>
#include <iow/workflow/timer_manager_base.hpp>

#include <string>
#include <ctime>
#include <time.h>
#include <iomanip>
#include <locale>
#include <chrono>
#include <mutex>

namespace iow{

template<typename Queue>
class timer_manager
  : public timer_manager_base
{
  typedef timer_manager_base super;
public:
  typedef timer_manager<Queue> self;
  typedef Queue queue_type;
  typedef std::shared_ptr<queue_type> queue_ptr;
  typedef std::function<bool()> handler;
  typedef std::function<void(bool)> handler_callback;
  typedef std::function<void(handler_callback)> async_handler;

  typedef std::chrono::system_clock         clock_t;
  typedef std::chrono::time_point<clock_t>  time_point_t;
  typedef time_point_t::duration            duration_t;
  typedef int timer_id_t;

  typedef std::mutex mutex_type;
  typedef std::weak_ptr<bool> wflag_type;
  typedef std::map< timer_id_t, std::shared_ptr<bool> > id_map;

  timer_manager( queue_ptr queue );/*
    : _queue(queue)
  {}*/

  // timers
  
  timer_id_t create( duration_t delay, handler h, bool expires_after = true);

  timer_id_t create( duration_t delay, async_handler h, bool expires_after = true);

  timer_id_t create(duration_t start_delay, duration_t delay, handler h, bool expires_after = true);

  timer_id_t create(duration_t start_delay, duration_t delay, async_handler h, bool expires_after = true);

  timer_id_t create(time_point_t start_time, duration_t delay, handler h, bool expires_after = true);

  timer_id_t create(time_point_t start_time, duration_t delay, async_handler h, bool expires_after = true);

  timer_id_t create(const std::string& start_time, duration_t delay, handler h, bool expires_after = true);

  timer_id_t create(const std::string& start_time, duration_t delay, async_handler h, bool expires_after = true);

  /// Evry 24 hours
  
  timer_id_t create(std::string start_time, handler h, bool expires_after = true);

  timer_id_t create(std::string start_time, async_handler h, bool expires_after = true);

  // reqesters 
  
  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create( duration_t d, std::shared_ptr<I> i, MemFun mem_fun,  Handler result_handler )
  {
    // в отличие от таймера, первый вызов немедленно 
    return this->create( std::chrono::milliseconds(0), d, this->make_reqester_<Req, Res>(i, mem_fun, result_handler));
  }


  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create( duration_t sd, duration_t d, std::shared_ptr<I> i, MemFun mem_fun,  Handler result_handler )
  {
    return this->create(sd, d, this->make_reqester_<Req, Res>(i, mem_fun, result_handler));
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create( time_point_t st, duration_t d, std::shared_ptr<I> i, MemFun mem_fun,  Handler result_handler )
  {
    return this->create(st, d, this->make_reqester_<Req, Res>(i, mem_fun, result_handler));
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create( std::string st, duration_t d, std::shared_ptr<I> i, MemFun mem_fun,  Handler result_handler )
  {
    return this->create(st, d, this->make_reqester_<Req, Res>(i, mem_fun, result_handler));
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create( std::string st, std::shared_ptr<I> i, MemFun mem_fun,  Handler result_handler )
  {
    return this->create(st, this->make_reqester_<Req, Res>(i, mem_fun, result_handler));
  }


  /// //////////////

private:

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  auto make_reqester_( std::shared_ptr<I> i, MemFun mem_fun, Handler result_handler ) 
    -> std::function<void( handler_callback)>
  {
    return requester::make<Req, Res>( this->_queue, i, std::move(mem_fun), std::move(result_handler) );
  }

  template<typename Handler>
  timer_id_t create_(time_point_t start_time, duration_t delay, Handler h, bool expires_after);

  template<typename Handler>
  timer_id_t create_(const std::string& start_time, duration_t delay, Handler h, bool expires_after);

private:

  queue_ptr  _queue;
};

}
