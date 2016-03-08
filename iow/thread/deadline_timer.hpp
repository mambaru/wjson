#pragma once

#include <iow/asio.hpp>
#include <iow/boost.hpp>
#include <iow/system.hpp>

#include <chrono>
#include <memory>
#include <functional>

namespace iow{
  
class deadline_timer
{
public:
  typedef ::iow::asio::deadline_timer timer_type;
  typedef ::iow::asio::io_service io_service;
public:
  deadline_timer( deadline_timer const & ) = delete;
  void operator=( deadline_timer const & ) = delete;

  deadline_timer(io_service& io)
    : _io(io)
  {}
  
  io_service& get_io_service() { return _io;}
  const io_service& get_io_service() const { return _io;}
  
  void run()
  {
    _io.run();
  }
  
  bool run_one()
  {
    return 0!=_io.run_one();
  }
  
  bool poll_one()
  {
    return 0!=_io.poll_one();
  }
  
  void stop()
  {
    _io.stop();
  }
  
  
  template<typename F>
  void post( F f )
  {
    _io.post( std::move(f) );
  }
  
  template<typename D, typename F>
  void delayed_post(D d, F f)
  {
    auto ptimer = std::make_shared<timer_type>(_io);

    time_t now_time = time(0);
    tm tms;
    localtime_r(&now_time, &tms);
    time_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
    
    auto pt = ::iow::asio::time_traits< ::boost::posix_time::ptime >::now();
    pt -= ::boost::posix_time::seconds( tms.tm_gmtoff );

    ptimer->expires_at(pt);
    ptimer->expires_from_now( ::boost::posix_time::milliseconds( ms ) );
    ptimer->async_wait([f, ptimer]( const ::iow::system::error_code& ){
      f();
    });
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
  io_service& _io;
};

} // iow

