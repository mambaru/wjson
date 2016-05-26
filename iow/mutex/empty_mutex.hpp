//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

namespace iow{
  
class empty_mutex
{

public:
  empty_mutex(){}

  empty_mutex(const empty_mutex&) = delete;
  empty_mutex& operator=(const empty_mutex&) = delete;

  void lock(){}
    
  void unlock(){}

  void lock_r() {}
  
  void unlock_r() {}
};

  
}
