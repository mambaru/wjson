#pragma once

#include <iow/workflow/queue_options.hpp>

#include <iow/asio.hpp>
#include <iow/boost.hpp>
#include <iow/system.hpp>
#include <iow/logger/logger.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <functional>
#include <mutex>

namespace iow{
  
class timed_queue
  : public std::enable_shared_from_this<timed_queue >
{
public:
  typedef timed_queue self;
  typedef ::iow::asio::deadline_timer timer_type;
  typedef std::shared_ptr<timer_type> timer_ptr;
  typedef ::iow::asio::io_service io_service;
  typedef std::mutex mutex_type;
  
public:
  
  timed_queue( timed_queue const & ) = delete;
  void operator=( timed_queue const & ) = delete;

  timed_queue(io_service& io, const queue_options& opt)
    : _io(io)
  {
    _counter = 0;
    _wrnsize = opt.wrnsize;
    _maxsize = opt.maxsize;
  }
  
  void reconfigure(const queue_options& opt)
  {
    _wrnsize = opt.wrnsize;
    _maxsize = opt.maxsize;
    
    std::lock_guard<mutex_type> lk(_handler_mutex);
    _wrn_handler = opt.wrn_handler;
    _max_handler = opt.max_handler;
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
 
  
  template<typename TP, typename F>
  bool post_at(TP tp, F f)
  {
    auto ptimer = this->create_timer_( tp );
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
  
  template<typename D, typename F>
  bool delayed_post(D duration, F f)
  {
    return this->post_at( std::move( std::chrono::system_clock::now() + duration  ), std::move(f) );
  }
  
  
  std::size_t size() const
  {
    return _counter;
  }
  
private:
  
  bool check_size_() const
  {
    if ( _counter < _wrnsize )
      return true;

    std::lock_guard<mutex_type> lk(_handler_mutex);
    if ( _counter < _maxsize )
    {
      if ( _wrn_handler == nullptr )
      {
        IOW_LOG_WARNING("timed_queue overflow warning size = " << _counter << "( wrnsize=" << _wrnsize << ")");
      }
      else
      {
        _wrn_handler(_counter, _wrnsize);
      }
      return true;
    }
    
    if ( _max_handler == nullptr )
    {
      IOW_LOG_ERROR("timed_queue overflow size = " << _counter << "( maxsize=" << _maxsize << ")");
    }
    else
    {
      _wrn_handler(_counter, _maxsize);
    }
    return false;
  }

  template<typename TP>
  timer_ptr create_timer_(TP tp)
  {
    typedef std::chrono::microseconds duration_t; 
    typedef duration_t::rep rep_t; 
    rep_t d = std::chrono::duration_cast<duration_t>(tp.time_since_epoch()).count(); 
    rep_t sec = d/1000000; 
    rep_t mksec = d%1000000; 
    ::boost::posix_time::ptime ptime = 
      ::boost::posix_time::from_time_t(0)
      + ::boost::posix_time::seconds(static_cast<long>(sec))
      + ::boost::posix_time::microseconds(mksec);
    //std::cout << std::endl << ptime << std::endl << sec << ":" << d<< std::endl;
    return std::make_shared<timer_type>( this->_io, ptime);
  }

private:
  io_service& _io;
  std::atomic<size_t> _counter;
  std::atomic<size_t> _maxsize;
  std::atomic<size_t> _wrnsize;
  
  mutable mutex_type _handler_mutex;
  queue_options::handler_type _wrn_handler;
  queue_options::handler_type _max_handler;
};

} // iow

