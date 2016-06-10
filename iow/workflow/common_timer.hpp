#pragma once

#include <iow/memory.hpp>
#include <chrono>

namespace iow{

class common_timer
{
public:
  typedef std::weak_ptr<bool> wflag_type;

  typedef std::function<bool()> handler;
  typedef std::function<void(bool)> handler_callback;
  typedef std::function<void(handler_callback)> async_handler;

  typedef std::chrono::system_clock         clock_t;
  typedef std::chrono::time_point<clock_t>  time_point_t;
  typedef time_point_t::duration            duration_t;


  template<typename Q, typename Handler>
  static std::function<void()> make( std::shared_ptr<Q> pq, duration_t delay, Handler handler, bool expires_after, wflag_type wflag )
  {
    std::weak_ptr<Q> wq = pq;
    return common_timer::create_handler_(wq, delay, expires_after, std::move(handler), wflag);
  }

private:

  template<typename Q, typename Handler>
  static std::function<void()> create_handler_(std::weak_ptr<Q> wq, duration_t delay, bool expires_after, Handler handler, wflag_type wflag)
  {
    return [wq, delay, expires_after, handler, wflag]()
    {
      if ( expires_after )
      {
        common_timer::expires_after_(wq, delay, std::move(handler), wflag );
      }
      else
      {
        common_timer::expires_before_(wq, delay, std::move(handler), wflag );
      }
    };
  }

  template<typename Q>
  static void expires_after_(std::weak_ptr<Q> wq, duration_t delay, handler handler, wflag_type wflag)
  {
    auto pflag = wflag.lock();
    if ( pflag == nullptr )
      return;

    if ( *pflag==false || handler() )
    {
      if ( auto pq = wq.lock() )
      {
        pq->delayed_post(delay, [wq, delay, handler, wflag]()
        {
          common_timer::expires_after_(wq, delay, std::move(handler), wflag );
        });
      }
    }
  }

  template<typename Q>
  static void expires_after_(std::weak_ptr<Q> wq, duration_t delay, async_handler handler, wflag_type wflag)
  {
    auto pflag = wflag.lock();
    if ( pflag == nullptr )
      return;

    auto post_handler = [wq, delay, handler, wflag]()
    {
      common_timer::expires_after_(wq, delay, std::move(handler), wflag );
    };
    
    if ( *pflag )
    {
      handler( [wq, delay, handler, wflag, post_handler](bool ready)
      {
        if ( !ready )
          return;
        
        if ( auto pq = wq.lock() )
        {
          pq->delayed_post(delay, post_handler);
        }
      });
    }
    else
    {
      if ( auto pq = wq.lock() )
      {
        pq->delayed_post(delay, post_handler);
      }
    }
  }

  template<typename Q>
  static void expires_before_(std::weak_ptr<Q> wq, duration_t delay, handler handler, wflag_type wflag)
  {
    auto pflag = wflag.lock();
    if ( pflag == nullptr )
      return;

    auto pq = wq.lock();
    if ( pq == nullptr )
      return;

    std::shared_ptr<bool> pres = std::make_shared<bool>(true); /// ???
    std::weak_ptr<bool> wres = pres;
    
    pq->delayed_post(delay, [wq, delay, handler, wflag, wres]()
    {
      if ( wres.lock() == nullptr )
        return;
      common_timer::expires_before_(wq, delay, std::move(handler), wflag );
    });
    
    if ( *pflag==false || !handler() )
    {
      pres.reset();
    }
  }

  template<typename Q>
  static void expires_before_(std::weak_ptr<Q> wq, duration_t delay, async_handler handler, wflag_type wflag)
  {
    auto pflag = wflag.lock();
    if ( pflag == nullptr )
      return;

    auto pq = wq.lock();
    if ( pq == nullptr )
      return;

    std::shared_ptr<bool> pres = std::make_shared<bool>(true);  // ???
    std::weak_ptr<bool> wres = pres;

    pq->delayed_post(delay, [wq, delay, handler, wflag, wres]()
    {
      if ( wres.lock() == nullptr )
        return;
      
      common_timer::expires_before_(wq, delay, std::move(handler), wflag );
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
};

}
