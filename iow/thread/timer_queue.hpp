#pragma once

#include <iow/thread/queue_options.hpp>

#include <iow/asio.hpp>
#include <iow/boost.hpp>
#include <iow/system.hpp>
#include <iow/logger/logger.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <functional>

namespace iow{
  
class timer_queue
  : public std::enable_shared_from_this<timer_queue >
{
public:
  typedef timer_queue self;
  typedef ::iow::asio::deadline_timer timer_type;
  typedef ::iow::asio::io_service io_service;
public:
  timer_queue( timer_queue const & ) = delete;
  void operator=( timer_queue const & ) = delete;

  timer_queue(io_service& io, const queue_options& opt)
    : _io(io)
    , _opt(opt)
  {
    _counter = 0;
  }
  
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
  bool post( F f )
  {
    if ( !this->check_size_() )
      return false;
      
    std::weak_ptr<self> wthis = this->shared_from_this();
    ++_counter;
    _io.post( [wthis, f]()
    {
      if (auto pthis = wthis.lock() )
      {
        --pthis->_counter;
        f();
      }
    } );
    return true;
  }
  
  template<typename D, typename F>
  bool delayed_post(D d, F f)
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

    if ( !this->check_size_() )
      return false;

    std::weak_ptr<self> wthis = this->shared_from_this();
    ++_counter;
    ptimer->async_wait([f, ptimer, wthis]( const ::iow::system::error_code& )
    {
      if (auto pthis = wthis.lock() )
      {
        --pthis->_counter;
        f();
      }
    });
    
    return true;
  }
  
  bool check_size_() const
  {
    if ( _counter < _opt.wrnsize )
      return true;

    if ( _counter < _opt.maxsize )
    {
      IOW_LOG_WARNING("timer_queue overflow warning size = " << _counter << "( wrnsize=" << _opt.wrnsize << ")");
      return true;
    }
    
    IOW_LOG_ERROR("timer_queue overflow size = " << _counter << "( maxsize=" << _opt.maxsize << ")");
    return false;
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
  queue_options _opt;
  std::atomic<size_t> _counter;
};

} // iow

