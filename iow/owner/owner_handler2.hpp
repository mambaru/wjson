//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/logger/logger.hpp>
#include <utility>
#include <memory>


namespace iow{

template<typename H, typename NA >
struct owner_handler2
{
  typedef std::weak_ptr<int> weak_type;

  owner_handler2() = default;

  owner_handler2(H&& h, NA&& nh,  weak_type alive)
    : _handler(  std::forward<H>(h) )
    , _alt_handler(  std::forward<NA>(nh) )
    , _alive(alive)
  {
  }
  
  template <class... Args>
  auto operator()(Args&&... args)
    ->  typename std::result_of< H(Args&&...) >::type
  {
    if ( auto p = _alive.lock() )
    {
      return _handler(std::forward<Args>(args)...);
    }
    /*
    typedef typename std::result_of< H(Args&&...) >::type result_type;
    typedef std::function< result_type(Args&&...) > func_type;
    func_type f = _alt_handler;*/
    IOW_LOG_DEBUG("owner_handler2() мимо 2")
    //return f( std::forward<Args>(args)... );
    abort();
    return _alt_handler(std::forward<Args>(args)...);
  }
private:
  H _handler;
  NA _alt_handler;
  weak_type _alive;
};

}
