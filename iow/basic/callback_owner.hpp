//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <memory>

namespace iow{
  
template<typename H, typename NA >
struct callback_wrapper
{
  H _handler;
  NA _not_alive;
  typedef std::weak_ptr<int> weak_type;
  
  callback_wrapper(H h, NA not_alive,  weak_type alive)
    : _handler( h )
    , _not_alive(not_alive )
    , _alive(alive)
  {
  }
  
  template <class... Args>
  auto operator()(Args&&... args)
    -> decltype( _handler(std::forward<Args>(args)...)) 
  {
    if ( auto p = _alive.lock() )
    {
      return _handler(std::forward<Args>(args)...);
    }
    
    return _not_alive(std::forward<Args>(args)...);
  }
private:
  weak_type _alive;
};

template<typename H>
struct callback_wrapper2
{
  H _handler;
  typedef std::weak_ptr<int> weak_type;
  
  callback_wrapper2(H h, weak_type alive)
    : _handler( h )
    , _alive(alive)
  {
  }
  
  template <class... Args>
  auto operator()(Args&&... args)
    -> decltype( _handler(std::forward<Args>(args)...)) 
  {
    if ( auto p = _alive.lock() )
    {
      return _handler(std::forward<Args>(args)...);
    }
    return decltype( _handler(std::forward<Args>(args)...))();
  }
private:
  weak_type _alive;
};


class callback_owner
{
  template <typename T>
  struct identity { typedef T type; };
public:
  
  typedef std::shared_ptr<int> alive_type;
  typedef std::weak_ptr<int>   weak_type;
  
  callback_owner()
    : _alive( std::make_shared<int>(1) )
  {
  }

  callback_owner(const callback_owner& ) = delete;
  callback_owner& operator = (const callback_owner& ) = delete;

  alive_type& alive() {return _alive;}
  const alive_type& alive() const {return _alive;}
  
  void reset()
  {
    _alive = std::make_shared<int>(1);
  }
  
  template<typename R, typename ... Args>
  std::function<R(Args&&...)> callback(typename identity<std::function<R(Args&&...)>>::type f)
  {
    std::weak_ptr<int> alive = _alive;

    return [alive,f](Args&&... args) -> R
    {
      if ( auto p = alive.lock() )
      {
        return f(std::forward<Args>(args)...);
      }
      return R();
    };
  }

  template<typename Handler, typename NotAliveHandler>
  callback_wrapper<Handler, NotAliveHandler>
  wrap(Handler handler, NotAliveHandler not_alive)
  {
    return callback_wrapper<Handler, NotAliveHandler>( handler, not_alive, _alive);
  }

  template<typename Handler>
  callback_wrapper2<Handler>
  wrap(Handler handler)
  {
    return callback_wrapper2<Handler>( handler, _alive);
  }

private:
  
  alive_type _alive;
};

}
