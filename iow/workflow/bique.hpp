#pragma once

#include <iow/asio.hpp>
#include <functional>
#include <chrono>

namespace iow{
  
class delayed_queue;
class asio_queue;
 
class bique
{
  typedef std::shared_ptr<delayed_queue> delayed_ptr;
  typedef std::shared_ptr<asio_queue> asio_ptr;
public:
  typedef ::iow::asio::io_service io_service_type;
  typedef std::function<void()>                               function_t;
  typedef std::chrono::time_point<std::chrono::system_clock>  time_point_t;
  typedef time_point_t::duration                              duration_t;
  
  virtual ~bique();
 
  bique( size_t maxsize);

  bique( io_service_type& io, size_t maxsize, bool use_asio/*= true*/ );
  
  void reconfigure(size_t maxsize, bool use_asio /*= true*/);

  void reset();

  bool run();
  
  bool run_one();
  
  bool poll_one();

  void stop();

  bool post( function_t f );

  bool post_at(time_point_t tp, function_t&& f);

  bool delayed_post(duration_t duration, function_t&& f);
  
  std::size_t size() const;
  
  std::size_t dropped() const;
  
private:
  
  template<typename R, typename... Args>
  R invoke_( 
    R(delayed_queue::* method1)(Args...),
    R(asio_queue::* method2)(Args...), 
    Args... args);

  template<typename R, typename... Args>
  R invoke_( 
    R(delayed_queue::* method1)(Args...) const,
    R(asio_queue::* method2)(Args...) const, 
    Args... args) const;

private:
  std::atomic<bool> _dflag;
  delayed_ptr _delayed;
  asio_ptr   _asio;
};

}
