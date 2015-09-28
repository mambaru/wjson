#pragma once

#include <iow/io/strand/aspect/aspect.hpp>
#include <iow/asio.hpp>
#include <iow/io/basic_io.hpp>
#include <mutex>

namespace iow{ namespace io{ namespace strand{

class holder
  : public ::iow::io::basic_io< ::iow::io::strand::aspect >
{
public:
  typedef ::iow::io::basic_io< ::iow::io::strand::aspect > super;
  typedef ::iow::asio::io_service io_service_type;
  typedef typename super::mutex_type mutex_type;
  
  holder(io_service_type& io)
    : _io_service(io)
  {
  }
  
  template<typename F>
  bool post(F&& f)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    return this->post_(*this, std::forward<F>(f));
    
  }

  template<typename F>
  auto wrap(F&& f)
    -> typename super::template result_of< iow::io::_wrap_, F>::type
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    return std::move( this->wrap_(*this, std::forward<F>(f) ) );
  }

  template<typename F>
  auto basic_wrap(F&& f)
    -> typename super::template result_of< iow::io::_wrap_, F>::type
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    return std::move( this->basic_wrap_(*this, std::forward<F>(f) ) );
  }

  template<typename O>
  void start(O&& opt)
  {
    std::lock_guard< mutex_type > lk(super::mutex() );
    super::start_(*this, std::forward<O>(opt));
  }

  template<typename O>
  void reconfigure(O&& opt)
  {
    std::lock_guard< mutex_type > lk(super::mutex());
    super::reconfigure_(*this, std::forward<O>(opt));
  }

  size_t size() const
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    return this->size_(*this);
  }

  io_service_type& get_io_service() { return _io_service;}

public:

  template<typename T>
  size_t size_(T& t) const
  {
    return t.get_aspect().template get<_size_>()(t);
  }

  template<typename T, typename F>
  bool post_(T& t, F&& f)
  {
    return t.get_aspect().template get<_post_>()(t, std::forward<F>(f) );
  }

  template<typename T, typename F>
  auto wrap_(T& t, F&& f)
    -> typename T::template result_of< iow::io::_wrap_, F>::type
  {
    return std::move( 
      t.get_aspect().template get<_wrap_>()(t, std::forward<F>(f) )
    );
  }

  template<typename T, typename F>
  auto basic_wrap_(T& t, F&& f)
    -> typename T::template result_of< iow::io::_wrap_, F>::type
  {
    return super::wrap_(t, std::forward<F>(f));
  }

private:

  io_service_type& _io_service;

};

}}} // iow::io::strand
