//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <utility>
#include <memory>
#include <type_traits>

namespace iow{
  
template<typename H>
class owner_handler
{
public:
  
  typedef std::weak_ptr<int> weak_type;

  owner_handler() = default;
 
  owner_handler(H&& h, weak_type alive)
    : _handler( std::forward<H>(h) )
    , _alive(alive)
  {
  }
  
  template <class... Args>
  auto operator()(Args&&... args)
    ->  typename std::result_of< H(Args&&...) >::type
  {
    if ( auto p = _alive.lock() )
    {
      return _handler( std::forward<Args>(args)... );
    }
    return typename std::result_of< H(Args&&...) >::type();
  }
  
private:
  H _handler;
  weak_type _alive;
};

}
