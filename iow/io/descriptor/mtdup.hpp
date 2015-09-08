#pragma once
#include <memory>
#include <mutex>
#include <list>
#include <thread>
#include <cstdlib>

namespace iow{ namespace io{ namespace descriptor{



template<typename Holder>
class mtdup
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

  mtdup(descriptor_type&& desc)
    : _origin( std::make_shared<holder_type>( std::forward<descriptor_type>(desc)))
  {
  }

  holder_ptr origin() const 
  { 
    return _origin;
  }
  
  template<typename Handler>
  auto wrap(Handler&& h)
    -> typename holder_type::template result_of<_wrap_, Handler>::type
  {
    return std::move( _origin->wrap( std::forward<Handler>(h)) );
  }


  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);

    if ( opt.threads == 0 )
    {
      _origin->start(opt);
      return;
    }

    for (int i = 0; i < opt.threads; ++i)
    {
      auto io = std::make_shared<io_service_type>();
      auto desc = _origin->template dup< descriptor_type >( *io );
      auto h = std::make_shared<holder_type>( std::move( desc ) );
      _dup_list.push_back(h);
      _services.push_back(io);

      h->start(opt);

      _threads.push_back( std::thread([io]()
      {
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

    _origin->close();
    for (auto h : _dup_list)
    {
      // сначала закрываем, чтоб реконнект на другой ассептор не прошел 
      h->close();
    }

    _origin->stop();
    for (auto h : _dup_list)
    {
      h->stop();
    }

    for (auto s : _services)
    {
      s->stop();
    }

    for (auto& t : _threads)
    {
      t.join();
    }

    _dup_list.clear();
    _threads.clear();
    _services.clear();
  }

  mutex_type& mutex() const
  {
    return _mutex;
  }

private:

  mutable mutex_type _mutex;
  holder_ptr _origin;
  holder_list _dup_list;
  thread_list _threads;
  service_list _services;
};

}}}
