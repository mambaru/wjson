#pragma once

#include <iow/io/strand/holder.hpp>
#include <iow/asio.hpp>
#include <cassert>
#include <thread>
#include <mutex>
#include <list>
#include <memory>

namespace iow{ namespace io{ namespace strand{

template<typename Holder = holder>
class mtholder
{
public:
  typedef Holder holder_type;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef std::shared_ptr<io_service_type> io_service_ptr;
  typedef std::vector<holder_ptr> holder_list;
  typedef std::vector<std::thread> thread_list;
  typedef std::vector<io_service_ptr> service_list;
  

  typedef std::mutex mutex_type;

  mtholder(io_service_type& io)
    : _io_ref(io)
  {
  }

  // TODO: reconfigure

  template<typename Opt>
  void start(Opt&& opt)
  {
    this->stop();

    std::lock_guard<mutex_type> lk(_mutex);

    if ( opt.disabled )
      return;

    if ( opt.threads == 0)
    {
      auto h = std::make_shared<holder_type>(_io_ref);
      _holders.push_back(h);
    }

    for ( size_t i=0; i < opt.threads; ++i )
    {
      auto s = std::make_shared<io_service_type>();
      auto h = std::make_shared<holder_type>(*s);
      _threads.push_back(std::thread([s]()
        {
          io_service_type::work wrk(*s);
          s->run();
          IOW_LOG_MESSAGE("strand thread finish " << std::this_thread::get_id())
        }));
      _holders.push_back(h);
      _services.push_back(s);
    }

    for (auto h: _holders)
    {
      h->start(std::forward<Opt>(opt));
    }

    /*
    if ( opt.threads != 0 )
    {
      _io_ptr = std::make_shared<io_service_type>();
    }
    else
    {
      _io_ptr = nullptr;
    }

    for ( size_t i = 0; !opt.disabled && i < opt.strands; ++i )
    {
      auto s = this->create_and_start_(std::forward<Opt>(opt));
      _strands.push_back( s );
    }

    if ( !_strands.empty() )
    {
      for ( size_t i = 0; i < opt.threads; ++i )
      {
        _threads.push_back(std::thread([this]()
        {
          io_service_type& io = *( this->_io_ptr );
          io_service_type::work wrk(io);
          io.run();
          IOW_LOG_MESSAGE("strand thread finish " << std::this_thread::get_id())
        }));
      }
    }
    */
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);

    for ( auto s : _services )
    {
      s->stop();
    }

    for ( auto h : _holders ) 
    {
      h->stop();
    }

    for ( std::thread& t : _threads ) 
    {
      t.join();
    }

    _threads.clear();
    _holders.clear();
    _services.clear();
  }

  bool enabled() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return !_holders.empty();
  }
  
  template<typename F>
  bool post(F&& f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _holders.empty() )
    {
      f();
      return true;
    }

    typedef typename holder_list::iterator iterator;
    iterator itr = _holders.begin();
    auto beg = itr;
    auto end = _holders.end();
    for ( ++beg ;beg!=end; ++beg)
    {
      if ( (*beg)->size() < (*itr)->size() )
      {
        itr = beg;
      }
    }
    //std::cout << std::distance(_strands.begin(), itr ) << " - " << _strands.size() << std::endl;
/*#warning

    auto& io = (*itr)->get_io_service();
    std::function<void()> ff = f;
    io.post( ff );
    return true;*/
    return (*itr)->post( std::forward<F>(f));
  }

  size_t size() const
  {
    size_t size = 0;
    for (auto &h : _holders)
    {
      size += h->size();
    }
    return size;
  }
  
private:
  
  /*
  template<typename Opt>
  holder_ptr create_and_start_(Opt&& opt)
  {
    holder_ptr pholder;
    if ( _io_ptr!=nullptr ) 
    {
      pholder = std::make_shared< holder_type>(*_io_ptr);
    }
    else 
    {
      pholder = std::make_shared< holder_type>(_io_ref);
    }
    pholder->start( std::forward<Opt>(opt) );
    return pholder;
  }
  */

private:
  io_service_type& _io_ref;
  thread_list _threads;
  holder_list _holders;
  service_list _services;
  mutable mutex_type _mutex;
};

}}} // iow::io::strand
