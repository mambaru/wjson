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
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef std::list<holder_ptr> holder_list;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::shared_ptr<io_service_type> io_service_ptr;
  typedef std::mutex mutex_type;

  mtholder(io_service_type& io)
    : _io_ref(io)
  {
  }
  
  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);

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
          IOW_LOG_MESSAGE("strand thread finish")
        }));
      }
    }
  }
  
  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if (_io_ptr!=nullptr)
    {
      _io_ptr->stop();
    }
    
    for ( auto s : _strands ) 
    {
      s->stop();
    }
    
    for ( std::thread& t : _threads ) 
    {
      t.join();
    }
  }
  
  bool enabled() const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return !_strands.empty();
  }
  
  template<typename F>
  bool post(F&& f)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _strands.empty() )
    {
      f();
      return true;
    }

    typedef typename holder_list::iterator iterator;
    iterator itr = _strands.begin();
    auto beg = itr;
    auto end = _strands.end();
    for ( ++beg ;beg!=end; ++beg)
    {
      if ( (*beg)->size() < (*itr)->size() )
      {
        itr = beg;
      }
    }
    
    return (*itr)->post( std::forward<F>(f));
  }

  
  size_t size() const
  {
    size_t size = 0;
    for (auto &s : _strands)
    {
      size += s->size();
    }
    return size;
  }
  
private:
  
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

private:
  typedef std::list<std::thread> thread_list;
  thread_list _threads;
  holder_list _strands;
  io_service_type& _io_ref;
  io_service_ptr _io_ptr;
  mutable mutex_type _mutex;
};

}}} // iow::io::strand
