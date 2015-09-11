#pragma once
#include <memory>
#include <mutex>
#include <list>
#include <thread>
#include <cstdlib>

#include <iow/logger/logger.hpp>
namespace iow{ namespace io{ namespace descriptor{



template<typename Holder>
class mtholder
{
public:
  typedef Holder holder_type;
  typedef typename holder_type::descriptor_type descriptor_type;
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef std::list<holder_ptr> holder_list;
  typedef std::list<std::thread> thread_list;
  typedef std::recursive_mutex mutex_type;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::shared_ptr<io_service_type> io_service_ptr;
  typedef std::list<io_service_ptr> service_list;

  mtholder(io_service_type& io)
    : _started(false)
    , _io_service(io)
  {
  }

  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    
    if (_started)
    {
      return;
    }
    
    _started = true;

    if ( opt.threads == 0 )
    {
      //descriptor_type desc(_io_service);
      //auto h = std::make_shared<Holder>( std::move(desc) );
      auto h = std::make_shared<Holder>( _io_service );
      _holder_list.push_back(h);
      h->start(opt);
      return;
    }

    for (int i = 0; i < opt.threads; ++i)
    {
      auto io = std::make_shared<io_service_type>();
      //descriptor_type desc(*io);
      //auto h = std::make_shared<holder_type>( std::move( desc ) );
      auto h = std::make_shared<holder_type>( *io );
      _holder_list.push_back(h);
      _services.push_back(io);
      _threads.push_back( std::thread([io, h, opt]()
      {
        h->start(opt);
        io->run();
      }));
    }
  }

  template<typename Opt>
  void reconfigure(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    std::abort();
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);

    if ( !_started )
    {
      return;
    }
    
    _started = false;

    IOW_LOG_DEBUG("mtdup::stop: _origin->close()")
    for (auto h : _holder_list)
    {
      // сначала закрываем, чтоб реконнект на другой ассептор не прошел 
      IOW_LOG_DEBUG("mtdup::stop: dup->close()")
      h->close();
    }

    IOW_LOG_DEBUG("mtdup::stop: _origin->stop()")
    for (auto h : _holder_list)
    {
      IOW_LOG_DEBUG("mtdup::stop: dup->stop()")
      h->stop();
    }

    for (auto s : _services)
    {
      IOW_LOG_DEBUG("mtdup::stop: service->stop()")
      s->stop();
    }

    for (auto& t : _threads)
    {
      IOW_LOG_DEBUG("mtdup::stop: thread.join()")
      t.join();
    }

    IOW_LOG_DEBUG("mtdup::stop: clear all")

    _holder_list.clear();
    _threads.clear();
    _services.clear();
    IOW_LOG_DEBUG("mtdup::stop: DONE")
  }

  mutex_type& mutex() const
  {
    return _mutex;
  }

private:
  bool _started;
  io_service_type& _io_service;
  mutable mutex_type _mutex;
  holder_list _holder_list;
  thread_list _threads;
  service_list _services;
};

}}}
