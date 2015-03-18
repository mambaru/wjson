//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/owner/owner_handler.hpp>
#include <iow/owner/owner_handler2.hpp>
#include <memory>

namespace iow{
  
class owner
{
public:
  
  typedef std::shared_ptr<int> alive_type;
  typedef std::weak_ptr<int>   weak_type;
  
  owner() 
  : _alive( std::make_shared<int>(1) ) 
  {
  }

  owner(const owner& ) = delete;
  owner& operator = (const owner& ) = delete;

  owner(owner&& ) = default;
  owner& operator = (owner&& ) = default;

  alive_type& alive() {return _alive;}
  const alive_type& alive() const {return _alive;}
  
  void reset()
  {
    _alive = std::make_shared<int>(1);
  }
  
  /*
  template<typename Handler>
  struct handler
  {
    typedef owner_handler< typename std::remove_reference<Handler>::type > type;
  };
  */
  
  template<typename Handler>
  owner_handler<typename std::remove_reference<Handler>::type>
  //owner_handler<Handler>
  wrap(Handler&& h)
  {
    //return owner_handler< Handler >( handler, _alive);
    return std::move(owner_handler< typename std::remove_reference<Handler>::type >( std::forward<Handler>(h), _alive));
  }

  template<typename Handler, typename NotAliveHandler>
  owner_handler2< typename std::remove_reference<Handler>::type, typename std::remove_reference<NotAliveHandler>::type>&&
  //owner_handler2< Handler, NotAliveHandler>
  wrap(Handler&& h, NotAliveHandler&& nh)
  {
    //return owner_handler2< Handler, NotAliveHandler>( handler, not_alive, _alive);
    return std::move(
      owner_handler2< 
	typename std::remove_reference<Handler>::type, 
	typename std::remove_reference<NotAliveHandler>::type
      >( std::forward<Handler>(h), std::forward<NotAliveHandler>(nh), _alive)
    );
  }

private:

  alive_type _alive;
};

}
