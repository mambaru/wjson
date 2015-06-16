//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once 

#include <atomic>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <condition_variable>

namespace wfc{
  
template<typename M = std::mutex>
class rwlock
{
  std::atomic<int> _readers;
  std::atomic<int> _writer;
  
  typedef M mutex_type;
  mutex_type _mutex;

  typedef std::lock_guard<mutex_type> lock_guard;
  typedef std::unique_lock<mutex_type> unique_lock;
  
  std::condition_variable_any _can_read;
  std::condition_variable_any _can_write;

  
public:
  rwlock()
    : _readers(0)
    , _writer(0)
    , _can_read()
    , _can_write()
  {
  }

  rwlock(const rwlock&) = delete;
  
  rwlock& operator=(const rwlock&) = delete;

  void lock()
  {
    unique_lock lk(_mutex);
    _can_write.wait(lk, [this]() { return this->_writer==0 && this->_readers==0; });
    ++_writer;
    if (_writer!=1)
      abort();
    if ( this->_readers!=0 )
      abort();
  }
    
  void unlock()
  {
    lock_guard lk(_mutex);
    --_writer;
    if (_writer!=0)
      abort();
    _can_write.notify_all();
    _can_read.notify_all();
  }

  void lock_r()
  {
    unique_lock lk(_mutex);
    _can_read.wait(lk, [this](){return this->_writer==0; });
    if ( this->_writer!=0 )
      abort();
    ++_readers;
  }
  
  void unlock_r()
  {
    lock_guard lk(_mutex);
    if ( this->_readers==0 )
      abort();
    --_readers;
    if ( _readers == 0)
      _can_write.notify_all();
  }
};

template<typename Mutex>
class read_lock
{
  
public:
  
  read_lock(Mutex &lck)
    : _mutex(lck)
  {
    _mutex.lock_r();
  }

  ~read_lock()
  {
    _mutex.unlock_r();
  }
  
  void lock()
  {
    _mutex.lock_r();
  }
  
  void unlock()
  {
    _mutex.unlock_r();
  }
  
private:
  
  Mutex &_mutex;
};

}
