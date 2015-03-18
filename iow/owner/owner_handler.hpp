//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <utility>
#include <memory>

namespace iow{
  
template<typename H>
struct owner_handler
{
  H _handler;
  typedef std::weak_ptr<int> weak_type;
  
  owner_handler(H&& h, weak_type alive)
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

}
