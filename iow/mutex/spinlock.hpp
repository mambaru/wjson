#pragma once

#include <atomic>
#include <functional>
#include <string>

namespace iow{
  
/**
 * спинлок на атомиках, совместимый с std. можно делать std::lock_guard<spinlock> (lck);
 */
class spinlock
{
	
public:
  spinlock()
    : value(false) { }

  spinlock(const spinlock&& other) 
  {
    value.exchange(other.value);
  }
  
  spinlock& operator=(const spinlock&& other) 
  {
    value.exchange(other.value);
    return *this;
  }
  
  spinlock(const spinlock&) = delete;
  
  spinlock& operator=(const spinlock&) = delete;

  void lock()
  {
    while (value.exchange(true)) { };  // now we have the lock
  }

  bool try_lock()
  {
    return !value.exchange(true);
  }

  void unlock()
  {
    value = false; // release lock
  }
  
private:
  
  std::atomic<bool> value;
};

class longspinlock : public spinlock {
}  __attribute__ ((aligned (32)));

template< size_t N >
class spinlock_n
{
  
public:

  typedef longspinlock mutex_type;
  
  mutex_type& get(const std::string& key)
  {
    return get( std::hash<std::string>()(key) );
  }

  mutex_type& get(size_t n)
  {
    return locs[n%N];
  }
  
private:
  longspinlock locs[N];
};

}