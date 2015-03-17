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

  alive_type& alive() {return _alive;}
  const alive_type& alive() const {return _alive;}
  
  void reset()
  {
    _alive = std::make_shared<int>(1);
  }
  
  template<typename Handler>
  struct handler
  {
    typedef owner_handler< typename std::remove_reference<Handler>::type > type;
  };
  
  template<typename Handler>
  owner_handler<typename std::remove_reference<Handler>::type>
  wrap(Handler handler)
  {
    return owner_handler< typename std::remove_reference<Handler>::type>( handler, _alive);
  }

  template<typename Handler, typename NotAliveHandler>
  owner_handler2< typename std::remove_reference<Handler>::type, NotAliveHandler>
  wrap(Handler handler, NotAliveHandler not_alive)
  {
    return owner_handler2< typename std::remove_reference<Handler>::type, NotAliveHandler>( std::move(handler), not_alive, _alive);
  }

private:

  alive_type _alive;
};

}
