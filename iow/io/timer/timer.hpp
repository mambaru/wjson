#pragma once

#include <iow/memory.hpp>
#include <iow/asio.hpp>
#include <iow/boost.hpp>
#include <iow/system.hpp>
#include <iow/logger/logger.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/time_traits.hpp>


#include <string>
#include <ctime>
#include <iomanip>
#include <locale>
#include <chrono>

namespace iow{ namespace io{

struct basic_timer_options
{
  bool expires_from_now = false;
  time_t delay_ms = 0;
};
  
struct timer_options: basic_timer_options
{
  time_t start_delay_ms = 0;
  std::string start_time;
};

class basic_timer
  : public std::enable_shared_from_this< basic_timer >
{
public:
  typedef timer_options options_type;
  typedef ::iow::asio::deadline_timer deadline_timer;
  typedef std::unique_ptr<deadline_timer> timer_ptr; 
  typedef std::function<bool()> handler1;
  typedef std::function<void(bool)> handler_callback;
  typedef std::function<void(handler_callback)> handler2;
  typedef ::boost::posix_time::ptime time_type;
  typedef std::function<time_type()> time_handler;
  
  basic_timer( deadline_timer& timer )
    : _timer(timer)
  {}

  template<typename Hundler>
  void start(Hundler fun, time_handler th, bool expires_from_now = true)
  {
    this->start_( std::move(fun), std::move(th), expires_from_now );
  }

  template<typename Hundler>
  void start(Hundler fun, time_t ms, bool expires_from_now = true)
  {
    this->start_( std::move(fun), std::move( this->make_time_handler(ms) ), expires_from_now );
  }

  template<typename Hundler>
  void proceed(Hundler fun, time_handler th, bool expires_from_now = true)
  {
    this->wait_( std::move(fun), std::move(th), expires_from_now );
  }

  template<typename Hundler>
  void proceed(Hundler fun, time_t ms, bool expires_from_now = true)
  {
    this->wait_( std::move(fun), std::move( this->make_time_handler(ms) ), expires_from_now );
  }

  void stop()
  {
    _timer.cancel();
  }
  
  time_handler make_time_handler(time_t ms)
  {
    using namespace ::boost::posix_time;
    ptime pt = basic_timer::now();
    return [pt, ms]() mutable -> time_type 
    {
      do 
      {
        pt += ::boost::posix_time::milliseconds(ms);
      } 
      while ( pt < basic_timer::now() );
      return pt;
    };
  }
  
  typedef ::iow::asio::time_traits<time_type> time_traits_t;
  
  static inline time_type now()
  {
    return time_traits_t::now();
  }
  
private:

  template<typename Fun>
  void start_( Fun fun, time_handler th, bool expires_from_now )
  {
    _timer.expires_at( th() );
    this->wait_( std::move(fun), std::move(th), expires_from_now);
  }

  template<typename Fun>
  void next_( Fun fun, time_handler th, bool expires_from_now )
  {
    if ( expires_from_now )
    {
      _timer.expires_at( th() );
    }
    this->wait_( std::move(fun), std::move(th), expires_from_now);
  }

  template<typename Fun>
  void wait_( Fun fun, time_handler th, bool expires_from_now )
  {
    std::weak_ptr<basic_timer> wthis = this->shared_from_this();
    _timer.async_wait( [wthis, fun, th, expires_from_now]( ::iow::system::error_code ec)
    {
      if ( !ec )
      {
        if (auto pthis = wthis.lock() )
        {
          if ( !expires_from_now )
          {
            pthis->_timer.expires_at( th() );
          }
          pthis->perform_( std::move(fun), std::move(th), expires_from_now );
        }
      }
      else
      {
        IOW_LOG_ERROR( "Timer error: " << ec.message() )
      }
    });
  }
  
  void perform_( handler1 fun, time_handler th, bool expires_from_now )
  {
    bool ready = true;
    try
    {
      ready = fun();
    }
    catch(...)
    {
      // TODO: LOG
    };
    
    if ( ready )
    {
      this->next_( std::move(fun), std::move(th), expires_from_now );
    }
    else
    {
      _timer.expires_from_now( ::boost::posix_time::seconds(0) );
      this->wait_( std::move(fun), std::move(th), expires_from_now);
    }
  }

  void perform_( handler2 fun, time_handler th, bool expires_from_now )
  {
    try
    {
      std::weak_ptr<basic_timer> wthis = this->shared_from_this();
      fun([wthis, fun, th, expires_from_now](bool ready){
        if (auto pthis = wthis.lock() )
        {
          if ( ready )
          {
            pthis->next_( std::move(fun), std::move(th), expires_from_now );
          }
          else
          {
            pthis->_timer.expires_from_now( ::boost::posix_time::seconds(0) );
            pthis->wait_( std::move(fun), std::move(th), expires_from_now);
          }
        }
      });
    }
    catch(...)
    {
      // TODO: LOG
      this->next_(std::move(fun), th, expires_from_now );
    };
    
  }

private:
  deadline_timer& _timer;
};

class timer
{
  typedef ::iow::asio::deadline_timer deadline_timer ;
public:
  
  typedef basic_timer::handler1 handler1;
  typedef basic_timer::handler_callback handler_callback;
  typedef basic_timer::handler2 handler2;
  
  timer( ::iow::asio::io_service& io)
    : _deadline_timer(io)
  {
  }

  // ::boost::posix_time::time_from_string("2005-12-07 23:59:59.000")
  // typedef std::function<void()> handler1;
  // typedef std::function<void(handler1)> handler2;
  // fun is handler1 or handler2
  template<typename Handler>
  void start(Handler fun, timer_options opt)
  {
    _timer=std::make_shared<basic_timer>(_deadline_timer);
    
    if ( opt.start_time.empty() && opt.start_delay_ms == 0 )
    {
      _timer->start(fun, opt.delay_ms, opt.expires_from_now);
    }
    else if ( !opt.start_time.empty() )
    {
      time_t rawtime;
      std::tm* ptm;
      time(&rawtime);
      ptm = localtime (&rawtime);
      if ( nullptr ==  strptime(opt.start_time.c_str(), "%H:%M:%S", ptm) )
      {
      }
      time_t now = time(0);
      time_t day_in_sec = 3600*24;
      time_t beg_day = (now/day_in_sec) * day_in_sec - ptm->tm_gmtoff;
      time_t ready_time_of_day = ptm->tm_sec + ptm->tm_min*60 + ptm->tm_hour*3600;
      time_t ready_time = beg_day + ready_time_of_day;
      if ( ready_time < now )
        ready_time += day_in_sec;

      _deadline_timer.expires_from_now( 
        ::boost::posix_time::seconds( ready_time - now ) 
        + ::boost::posix_time::milliseconds(opt.start_delay_ms)
      );
      _timer->proceed( fun, opt.delay_ms, opt.expires_from_now );
    }
    else
    {
      _deadline_timer.expires_from_now( ::boost::posix_time::milliseconds(opt.start_delay_ms) );
      _timer->proceed( fun, opt.delay_ms, opt.expires_from_now );
    }
  }

  void stop()
  {
    _timer->stop();
    _timer = nullptr;
  }

private:
  deadline_timer _deadline_timer;
  std::shared_ptr<basic_timer> _timer;
};

}}
