#pragma once

#include <iow/memory.hpp>
#include <iow/asio.hpp>
#include <iow/system.hpp>
#include <iow/logger/logger.hpp>
#include <iow/owner/owner.hpp>

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

  // timers
  
  template<typename Handler>
  timer_id_t create( duration_t delay, Handler handler, bool expires_after = true)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->create_( clock_t::now() + delay, delay, expires_after, std::forward<Handler>(handler) );
  }

  template<typename Handler>
  timer_id_t create(duration_t start_delay, duration_t delay, Handler handler, bool expires_after = true)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->create_( clock_t::now() + start_delay, delay, expires_after, std::forward<Handler>(handler) );
  }

  template<typename Handler>
  timer_id_t create(time_point_t start_time, duration_t delay, Handler handler, bool expires_after = true)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->create_( start_time, delay, expires_after, std::forward<Handler>(handler) );
  }

  template<typename Handler>
  timer_id_t create(std::string start_time, duration_t delay, Handler handler, bool expires_after = true)
  {
    return start_time.empty() 
      ? this->create(delay, std::move(handler), expires_after)
      : this->create( this->str2tp_( std::move(start_time) ), delay, std::move(handler),  expires_after);
  }

  /// Evry 24 hours
  template<typename Handler>
  timer_id_t create(std::string start_time, Handler handler, bool expires_after = true)
  {
    return this->create( std::move(start_time), std::chrono::milliseconds(0), std::forward<Handler>(handler), expires_after );
  }

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
  
  size_t size() 
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return _id_map.size();
  }

private:
  
  template< typename Req, typename Res, typename I, typename MemFun, typename Handler >
  auto make_reqester_( std::shared_ptr<I> i, MemFun mem_fun, Handler result_handler ) 
    -> std::function<void( handler_callback)>
  {
    std::weak_ptr<I> wi = i;
    std::weak_ptr< self > wthis = this->shared_from_this();
    return [wthis, wi, mem_fun, result_handler](handler_callback timer_handler)
    {
      if ( auto pthis = wthis.lock() )
      {
        auto req = result_handler( nullptr );
        pthis->send_request_<Res>( std::move(req), wi, mem_fun, std::move(result_handler), std::move(timer_handler) );
      }
    };
  }


  //template< typename Res, typename Req, typename I, void (I::*mem_ptr)( std::unique_ptr<Req>, std::function< void( std::unique_ptr<Res> ) > ) >
  template< typename Res, typename ReqPtr, typename I, typename MemFun, typename ResultHandler >
  void send_request_(ReqPtr req, std::weak_ptr<I> wi, MemFun mem_ptr, ResultHandler result_handler, handler_callback timer_handler)
  {
    auto i = wi.lock();
    if ( i == nullptr )
    {
      timer_handler(false);
      return;
    }

    std::weak_ptr< self > wthis = this->shared_from_this();

    auto callback = [ wthis, wi, mem_ptr, result_handler, timer_handler]( std::unique_ptr<Res> res)
    {

      if ( auto pthis = wthis.lock() )
      {
        if ( res == nullptr )
        {
          // is service unavailable
          timer_handler(true);
          return;
        }
        
        auto pres = std::make_shared< std::unique_ptr<Res> >( std::move(res) );
        pthis->_queue->post([pres, wthis, wi, mem_ptr, result_handler, timer_handler]()
        {
          if ( auto pthis = wthis.lock() )
          {
            if ( auto req = result_handler( std::move(*pres) ) )
            {
              pthis->send_request_<Res>( std::move(req), wi, mem_ptr, std::move(result_handler), std::move(timer_handler) );
            }
            else
            {
              timer_handler(true);
            }
          }
        });
      }
    };
    
    (i.get()->*mem_ptr)( std::move(req), callback );
  }
  
  
  time_point_t str2tp_(std::string start_time)
  {
    std::time_t now = std::time(0);
    std::tm ptm;
    localtime_r(&now, &ptm);
    
    if ( nullptr ==  strptime(start_time.c_str(), "%H:%M:%S", &ptm) )
    {
      IOW_LOG_ERROR("Timer format error (class timer_manager) %H:%M:%S: " << start_time );
      return std::chrono::system_clock::now();
    }
    
    time_t day_in_sec = 3600*24;
    time_t beg_day = (now/day_in_sec) * day_in_sec - ptm.tm_gmtoff;
    time_t ready_time_of_day = ptm.tm_sec + ptm.tm_min*60 + ptm.tm_hour*3600;
    time_t ready_time = beg_day + ready_time_of_day;
    if ( ready_time < now )
    ready_time += day_in_sec;
    return std::chrono::system_clock::from_time_t(ready_time);
  }

  
  template<typename Handler>
  timer_id_t create_(time_point_t start_time, duration_t delay, bool expires_after, Handler handler)
  {
    if ( delay.count() == 0 )
      delay = std::chrono::hours(24);
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
  
  owner _owner;
  queue_ptr  _queue;
  timer_id_t _id_counter;
  id_map     _id_map;
  mutable mutex_type _mutex;
  
};

}
