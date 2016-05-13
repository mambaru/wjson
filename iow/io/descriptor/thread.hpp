#pragma once
#include <memory>
#include <mutex>
#include <list>
#include <thread>
#include <cstdlib>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace descriptor{


template<typename Holder>
class thread
  : public std::enable_shared_from_this< thread<Holder> >
  
{
public:
  typedef Holder holder_type;
  /*typedef typename holder_type::data_ptr data_ptr;
  typedef typename holder_type::io_id_type io_id_type;
  typedef typename holder_type::descriptor_type descriptor_type;
  */
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef std::thread thread_type;
  typedef std::mutex mutex_type;
  typedef std::shared_ptr<thread_type> thread_ptr;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::shared_ptr<io_service_type> io_service_ptr;

  thread(io_service_type& io)
    : _started(false)
    , _io_service(io)
  {
  }

  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    this->start_( std::forward<Opt>(opt) );
  }

  template<typename Opt>
  void start_(Opt&& opt1)
  {
    typename std::decay<Opt>::type opt = opt1;
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _holder != nullptr ) return;

    if ( opt.threads != 0 )
      _io_service2 = std::make_shared<io_service_type>();
      
    _holder = std::make_shared<holder_type>( (_io_service2!=nullptr ? (*_io_service2) : _io_service) );
    _holder->start(opt1);
    if ( io_service_ptr io = this->_io_service2 )
    {
      _thread = std::thread([io]()
      {
        iow::system::error_code ec;
        io->run(ec);
      });
    }
  }

  template<typename Opt>
  void reconfigure(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _holder == nullptr ) return;
    bool f1 = (_io_service2 == nullptr);
    bool f2 = (opt.threads == 0);
    
    if ( f1 != f2)
    {
      this->stop_();
      return this->start_( std::forward<Opt>(opt) );
    }
    _holder->reconfigure( std::forward<Opt>(opt) );
  }
  
  template<typename Handler>
  void shutdown(Handler handler)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    if ( _holder == nullptr ) return;
    
    auto pthis = this->shared_from_this();
    _holder->shutdown([handler,pthis]()
    {
      pthis->stop();
      if (handler != nullptr) handler();
    });
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _holder == nullptr ) return;
    this->stop_();
  }
  
  holder_ptr holder() const 
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _holder;
  }
  
  void stop_()
  {
    _holder->stop();
    _holder = nullptr;
    if ( _io_service2 != nullptr )
    {
      _io_service2->stop();
      _io_service2 = nullptr;
      _thread.join();
    }
  }

  mutex_type& mutex() const
  {
    return _mutex;
  }

private:
  bool _started;
  io_service_type& _io_service;
  io_service_ptr _io_service2;
  mutable mutex_type _mutex;
  holder_ptr _holder;
  thread_type _thread;
};

}}}
