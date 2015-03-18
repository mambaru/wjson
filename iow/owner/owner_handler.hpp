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
struct owner_handler
{
  typedef std::weak_ptr<int> weak_type;

  owner_handler() = delete;

  
  owner_handler( const owner_handler<H>& other)
    : _handler(other._handler)
    , _alive(other._alive)
  {}

  owner_handler( owner_handler<H>&& other)    
    : _handler( std::move(other._handler))
    , _alive( other._alive )
  {
    //std::cout << "   owner_handler( owner_handler<H>&& other) " << std::endl;
  }

  void operator=( const owner_handler<H>& other)  
  {
    _handler = other._handler;
    _alive = other._alive;
  }

  
  void operator=( owner_handler<H>&& other)  
  {
    _handler = std::move(other._handler);
    _alive = other._alive;
  }
  

  owner_handler(H&& h, weak_type alive)
    : _handler( std::forward<H>(h) )
    , _alive(alive)
  {
  }
  
  
  template <class... Args>
  auto operator()(Args&&... args)
    //-> decltype( /*_handler*/std::result_of(H(std::forward<Args>(args)...)) 
    ->  typename std::result_of< H(Args&&...) >::type
  {
    if ( auto p = _alive.lock() )
    {
      // std::cout << "handler ptr: "<< (size_t)(&_handler) << std::endl;
      return _handler(std::forward<Args>(args)...);
    }
    //return decltype( _handler(std::forward<Args>(args)...))();
    return typename std::result_of< H(Args&&...) >::type();
  }
private:
  H _handler;
  weak_type _alive;
};

}
