//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <utility>
#include <memory>

namespace iow{
  
template<typename H, typename NA >
struct owner_handler2
{
  H _handler;
  NA _not_alive;
  typedef std::weak_ptr<int> weak_type;
  
  owner_handler2(H h, NA not_alive,  weak_type alive)
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

}
