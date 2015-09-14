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
  typedef typename holder_type::data_ptr data_ptr;
  typedef typename holder_type::descriptor_type descriptor_type;
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef std::list<holder_ptr> holder_list;
  typedef std::list<std::thread> thread_list;
  typedef std::recursive_mutex mutex_type;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::shared_ptr<io_service_type> io_service_ptr;
  typedef std::list<io_service_ptr> service_list;
  typedef typename holder_list::const_iterator holder_iterator;

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
      auto h = std::make_shared<Holder>( _io_service );
      _holder_list.push_back(h);
      h->start( std::forward<Opt>(opt));
    }

    for (int i = 0; i < opt.threads; ++i)
    {
      auto io = std::make_shared<io_service_type>();
      auto h = std::make_shared<holder_type>( *io );
      h->start(std::forward<Opt>(opt)); // Запускаем до потока, чтобы инициализировать
      _holder_list.push_back(h);
      _services.push_back(io);
      _threads.push_back( std::thread([io/*, h, opt*/]()
      {
        io->run();
      }));
    }
    _iterator = _holder_list.begin();
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
  
  holder_ptr next() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _holder_list.empty() )
    {
      return nullptr;
    }

    if ( _iterator == _holder_list.end() )
    {
        _iterator = _holder_list.begin();
    }
    return *(_iterator++);
  }

private:
  bool _started;
  io_service_type& _io_service;
  mutable mutex_type _mutex;
  holder_list _holder_list;
  mutable holder_iterator _iterator;
  thread_list _threads;
  service_list _services;
};

}}}
