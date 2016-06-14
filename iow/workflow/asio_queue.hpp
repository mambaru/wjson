#pragma once

#include <iow/asio.hpp>
#include <mutex>
#include <memory>
#include <atomic>

namespace iow{
  
class asio_queue
  : public std::enable_shared_from_this<asio_queue >
{
  typedef asio_queue self;
  typedef ::iow::asio::deadline_timer timer_type;
  typedef std::shared_ptr<timer_type> timer_ptr;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::mutex mutex_type;
public:
  typedef std::function<void()>                               function_t;
  typedef std::chrono::time_point<std::chrono::system_clock>  time_point_t;
  typedef time_point_t::duration                              duration_t;

  
  asio_queue( asio_queue const & ) = delete;
  void operator=( asio_queue const & ) = delete;

  asio_queue(io_service_type& io, size_t maxsize);
  
  void set_maxsize(size_t maxsize);

  io_service_type& get_io_service();
  
  const io_service_type& get_io_service() const;
  
  bool run();
  
  bool run_one();
  
  bool poll_one();
  
  void reset();
  
  void stop();
  
  bool post( function_t f );
 
  bool post_at(time_point_t tp, function_t f);
  
  bool delayed_post(duration_t duration, function_t f);
  
  std::size_t size() const;
  std::size_t dropped() const;
  
private:
  
  bool check_();
  
  template<typename TP>
  timer_ptr create_timer_(TP tp);

private:
  io_service_type& _io;
  std::atomic<size_t> _counter;
  std::atomic<size_t> _maxsize;
  std::atomic<size_t> _drop_count;
};

} // iow

