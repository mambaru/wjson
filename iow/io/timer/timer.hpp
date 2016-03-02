#pragma once

#include <iow/memory.hpp>
#include <iow/asio.hpp>
#include <iow/boost.hpp>
#include <iow/system.hpp>

#include <string>
#include <ctime>

namespace iow{ namespace io{

struct timer_options
{
  bool enabled = true;
  time_t start_delay_ms = 0;
  time_t delay_ms = 0;
  std::string start_time;
};

class timer
  : public std::enable_shared_from_this< timer >
{
public:
  typedef timer_options options_type;
  typedef ::iow::asio::deadline_timer deadline_timer;
  typedef std::unique_ptr<deadline_timer> timer_ptr; 
  typedef std::function<void()> handler1;
  typedef std::function<void(handler1)> handler2;
  
  
  timer( ::iow::asio::io_service& io, const options_type& opt )
    : _opt(opt)
  {
    if ( !opt.enabled )
      return;
    
    _timer = std::make_unique<deadline_timer>(io);
  }

  void reconfigure(const timer_options& opt)
  {
    _opt = opt;
    // TODO
  }
  
  void start(handler1 fun)
  {
    this->wait_( _opt.start_delay_ms, std::move(fun) );
  }

  void start(handler2 fun)
  {
    this->wait_( _opt.start_delay_ms, std::move(fun) );
  }

private:
  template<typename Fun>
  void wait_( time_t ms, Fun fun )
  {
    _timer->expires_from_now( ::boost::posix_time::milliseconds( ms ) );
    std::weak_ptr<timer> wthis = this->shared_from_this();
    _timer->async_wait( [wthis, fun]( ::iow::system::error_code ec)
    {
      if ( !ec )
      {
        if (auto pthis = wthis.lock() )
        {
          pthis->wait_handler_( std::move(fun) );
        }
      }
      else
      {
        DOMAIN_LOG_ERROR("Timer error: " << ec.message())
      }
    });
  }
  
  void wait_handler_( handler1 fun )
  {
    try
    {
      fun();
    }
    catch(...)
    {
      // TODO: LOG
    };
    this->wait_(_opt.delay_ms, std::move(fun) );
  }

  void wait_handler_( handler2 fun )
  {
    try
    {
      std::weak_ptr<timer> wthis = this->shared_from_this();
      fun([wthis, fun](){
        if (auto pthis = wthis.lock() )
        {
          pthis->wait_( pthis->_opt.delay_ms, std::move(fun) );
        }
      });
    }
    catch(...)
    {
      // TODO: LOG
      this->wait_(_opt.delay_ms, std::move(fun) );
    };
    
  }

private:
  
  options_type _opt;
  timer_ptr _timer;
};

}}
