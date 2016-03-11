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
#include <mutex>

namespace iow{

template<typename Queue>
class timer_manager
  : public std::enable_shared_from_this< timer_manager<Queue> >
{
public:
  typedef timer_manager<Queue> self;
  typedef Queue queue_type;
  typedef std::shared_ptr<queue_type> queue_ptr;
  typedef std::function<bool()> handler1;
  typedef std::function<void(bool)> handler_callback;
  typedef std::function<void(handler_callback)> handler2;
  
  typedef std::chrono::system_clock clock_t;
  typedef std::chrono::time_point<clock_t>  time_point_t;
  typedef time_point_t::duration                              duration_t;
  typedef int timer_id_t;
  
  typedef std::mutex mutex_type;
  typedef std::weak_ptr<bool> wflag_type;
  typedef std::map< timer_id_t, std::shared_ptr<bool> > id_map;
  
  timer_manager( queue_ptr queue )
    : _queue(queue)
    , _id_counter(0)
  {}

  template<typename Handler>
  timer_id_t create(time_point_t start_time, duration_t delay, Handler handler, bool expires_after = true)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->create_( start_time, delay, expires_after, std::move(handler) );
  }

  template<typename Handler>
  timer_id_t create( duration_t delay, Handler handler, bool expires_after = true)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->create_( clock_t::now() + delay, delay, expires_after, std::move(handler) );
  }

  std::shared_ptr<bool> detach(timer_id_t id)
  {
    std::shared_ptr<bool> res;
    std::lock_guard< mutex_type > lk(_mutex);
    auto itr = _id_map.find(id);
    if ( itr == _id_map.end() )
      return nullptr;
    res = itr->second;
    _id_map.erase(itr);
    return res;

  }
  
  bool release( timer_id_t id )
  {
    std::lock_guard< mutex_type > lk(_mutex);
    auto itr = _id_map.find(id);
    if ( itr == _id_map.end() )
      return false;
    _id_map.erase(itr);
    return true;
  }
  
  size_t clear()
  {
    std::lock_guard< mutex_type > lk(_mutex);
    size_t s = _id_map.size();
    _id_map.clear();
    return s;
  }
  
  template<typename Handler>
  timer_id_t create_(time_point_t start_time, duration_t delay, bool expires_after, Handler handler)
  {
    timer_id_t id = ++_id_counter;
    std::shared_ptr<bool> pflag = std::make_shared<bool>(true);
    std::weak_ptr<bool> wflag = pflag;
    _id_map.insert( std::make_pair(id, pflag) );
    this->_queue->post_at( start_time, this->create_handler_(delay, expires_after, std::move(handler), wflag) );
    return id;
  }
  
  template<typename Handler>
  std::function<void()> create_handler_(duration_t delay, bool expires_after, Handler handler, wflag_type wflag)
  {
    std::weak_ptr<self> wthis = this->shared_from_this();
    
    return [wthis, delay, expires_after, handler, wflag]()
    {
      if ( auto pthis = wthis.lock() )
      {
        if ( expires_after )
        {
          pthis->expires_after_(delay, std::move(handler), wflag );
        }
        else
        {
          pthis->expires_before_(delay, std::move(handler), wflag );
        }
      }
    };
  }
  
  void expires_after_(duration_t delay, handler1 handler, wflag_type wflag)
  {
    auto pflag = wflag.lock();
    if ( pflag == nullptr )
      return;

    if ( *pflag==false || handler() )
    {
      std::weak_ptr<self> wthis = this->shared_from_this();
      this->_queue->delayed_post(delay, [wthis, delay, handler, wflag]()
      {
        if ( auto pthis = wthis.lock() )
        {
          pthis->expires_after_(delay, std::move(handler), wflag );
        }
      });
    }
  }

  void expires_after_(duration_t delay, handler2 handler, wflag_type wflag)
  {
    auto pflag = wflag.lock();
    if ( pflag == nullptr )
      return;

    std::weak_ptr<self> wthis = this->shared_from_this();
    auto post_handler = [wthis, delay, handler, wflag]()
    {
      if ( auto pthis = wthis.lock() )
      {
        pthis->expires_after_(delay, std::move(handler), wflag );
      }
    };
    
    if ( *pflag )
    {
      handler( [wthis, delay, handler, wflag, post_handler](bool ready)
      {
        if ( !ready )
          return;
        
        if ( auto pthis = wthis.lock() )
        {
          pthis->_queue->delayed_post(delay, post_handler);
        }
      });
    }
    else
    {
      this->_queue->delayed_post(delay, post_handler);
    }
  }

  void expires_before_(duration_t delay, handler1 handler, wflag_type wflag)
  {
    auto pflag = wflag.lock();
    if ( pflag == nullptr )
      return;
    
    std::shared_ptr<bool> pres = std::make_shared<bool>(true);
    std::weak_ptr<bool> wres = pres;
    std::weak_ptr<self> wthis = this->shared_from_this();
    
    this->_queue->delayed_post(delay, [wthis, delay, handler, wflag, wres]()
    {
      if ( wres.lock() == nullptr )
        return;
      
      if ( auto pthis = wthis.lock() )
      {
        pthis->expires_before_(delay, std::move(handler), wflag );
      }
    });
    
    if ( *pflag==false || !handler() )
    {
      pres.reset();
    }
  }

  void expires_before_(duration_t delay, handler2 handler, wflag_type wflag)
  {
    auto pflag = wflag.lock();
    if ( pflag == nullptr )
      return;

    std::shared_ptr<bool> pres = std::make_shared<bool>(true);
    std::weak_ptr<bool> wres = pres;
    std::weak_ptr<self> wthis = this->shared_from_this();
    
    this->_queue->delayed_post(delay, [wthis, delay, handler, wflag, wres]()
    {
      if ( wres.lock() == nullptr )
        return;
      
      if ( auto pthis = wthis.lock() )
      {
        pthis->expires_before_(delay, std::move(handler), wflag );
      }
    });
    
    if ( *pflag==true )
    {
      handler( [wres](bool ready) mutable
      {
        if (!ready)
          wres.reset();
      });
    }
  }

private:

  queue_ptr  _queue;
  timer_id_t _id_counter;
  id_map     _id_map;
  mutable mutex_type _mutex;
};

}
