#pragma once

#include <iow/workflow/workflow_options.hpp>
#include <iow/workflow/task_manager.hpp>
#include <iow/workflow/bique.hpp>
#include <wfc/asio.hpp>
#include <chrono>


namespace iow{

class workflow
{
public:
  typedef ::wfc::asio::io_service io_service_type;
  typedef std::function< void() > post_handler;

  typedef std::function<bool()> timer_handler;
  typedef std::function<void(bool)> callback_timer_handler;
  typedef std::function<void(callback_timer_handler)> async_timer_handler;

  typedef std::chrono::system_clock         clock_t;
  typedef std::chrono::time_point<clock_t>  time_point_t;
  typedef time_point_t::duration            duration_t;
  typedef int                               timer_id_t;

  typedef task_manager::timer_type          timer_type;

public:
  virtual ~workflow();
  workflow(const workflow& ) = delete;
  workflow& operator=(const workflow& ) = delete;
  
  explicit workflow(workflow_options opt = workflow_options() );
  workflow(io_service_type& io, workflow_options opt = workflow_options() );
  
  std::shared_ptr<task_manager> get() const;
  std::shared_ptr<timer_type> get_timer() const;
  
  void start();
  void reconfigure(workflow_options opt);
  void stop();
  
  std::shared_ptr<bool> detach_timer(timer_id_t );
  bool release_timer( timer_id_t id );

  size_t timer_size() const;
  size_t queue_size() const;

  bool post(post_handler handler);
  bool post(time_point_t, post_handler handler);
  bool post(duration_t,   post_handler handler);

  timer_id_t create_timer(duration_t, timer_handler, bool = true);
  timer_id_t create_async_timer(duration_t, async_timer_handler, bool = true);

  timer_id_t create_timer(duration_t, duration_t, timer_handler, bool = true);
  timer_id_t create_async_timer(duration_t, duration_t, async_timer_handler, bool = true);

  timer_id_t create_timer(time_point_t, duration_t, timer_handler, bool = true);
  timer_id_t create_async_timer(time_point_t, duration_t, async_timer_handler, bool = true);

  timer_id_t create_timer(std::string, duration_t, timer_handler, bool = true);
  timer_id_t create_async_timer(std::string, duration_t, async_timer_handler, bool = true);
  
  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create_requester( duration_t d, std::shared_ptr<I> i, MemFun mem_fun, Handler handler )
  {
    return this->get_timer()->create<Req, Res>( d, i, mem_fun, std::move(handler) );
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create_requester( duration_t sd, duration_t d, std::shared_ptr<I> i, MemFun mem_fun, Handler handler )
  {
    return this->get_timer()->create<Req, Res>( sd, d, i, mem_fun, std::move(handler) );
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create_requester( std::string st, duration_t d, std::shared_ptr<I> i, MemFun mem_fun, Handler handler )
  {
    return this->get_timer()->create<Req, Res>(st, d, i, mem_fun, std::move(handler) );
  }

  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  timer_id_t create_requester( time_point_t tp, duration_t d, std::shared_ptr<I> i, MemFun mem_fun, Handler handler )
  {
    return this->get_timer()->create<Req, Res>( tp, d, i, mem_fun, std::move(handler) );
  }

private:
  void create_wrn_timer_(time_t ms, size_t wrnsize, size_t maxsize);
private:
  std::string _id;
  std::atomic<time_t> _delay_ms;
  std::shared_ptr<task_manager> _impl;
  timer_id_t _wrn_timer = 0;
  std::shared_ptr<workflow> _workflow_ptr;
};

}
