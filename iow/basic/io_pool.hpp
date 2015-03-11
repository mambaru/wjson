#pragma once

#include <iow/basic/callback_owner.hpp>
#include <iow/basic/io_id.hpp>
#include <iow/basic/io_thread.hpp>
#include <iow/basic/tags.hpp>
#include <iow/aux/asio.hpp>


#include <fas/aop.hpp>

#include <thread>
#include <mutex>

  
namespace iow{ 
  
 
template<typename Subject>
class io_pool
{
public:
  typedef io_pool<Subject, A> self;
  typedef Subject subject;
  typedef io_thread<Subject> io_thread_type;
  typedef std::list<io_thread_type> thread_list;
  

  typedef typename subject::io_service_type io_service_type;
  typedef typename subject::options_type options_type;
  typedef std::recursive_mutex mutex_type;
  
  io_pool(io_service_type& io_service, const options_type& opt )
  {
    if ( opt.threads == 0 )
    {
      _thread_list.push_back( io_thread_type(io_service, opt)  );
    }
    else
    {
      for ( decltype(opt.threads) i = 0 ; i < opt.threads; ++l )
      {
        _thread_list.push_back( io_thread_type(opt)  );
      }
    }
  }
  
  // TODO: set_options

  void start()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    for( auto& t: _thread_list) 
    {
      t.start();
    }
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    for( auto& t: _thread_list) 
    {
      t.stop();
    }
  }

public:

  thread_list _thread_list;
  mutex_type _mutex;
};

}
