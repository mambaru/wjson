#pragma once

#include <iow/basic/io_base.hpp>
#include <iow/basic/tags.hpp>
#include <iow/aux/asio.hpp>


#include <fas/aop.hpp>

#include <thread>
#include <mutex>

namespace iow{ 
  
template<typename A >
class io_strand_base
  : public io_base<A>
{
public:
  typedef io_strand_base<A> self;
  typedef io_base<A> super;
  typedef typename super::io_service_type io_service_type;
  typedef typename io_service_type::strand strand_type;
  
  typedef typename super::options_type options_type;
  typedef typename super::mutex_type mutex_type;
  
  ~io_strand_base()
  {
  }
  
  io_strand_base(io_service_type& io_service, const options_type& opt )
    : io_base<A>(io_service, opt)
    , _strand(io_service)
  {
  }

  strand_type& get_strand()
  {
    return _strand;
  }

  const strand_type& get_strand() const
  {
    return _strand;
  }
  

///  //////////////////////////////////////////////

  template<typename Handler>
  void post(Handler h)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    this->post_( std::move(h) );
  }

  template<typename Handler>
  Handler post_wrap(Handler h)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    return std::move( this->post_wrap_( std::move(h) ) );
  }
  
public:
  
  template<typename Handler>
  void post_(Handler h)
  {
    _strand.post( std::move( super::owner_wrap_( std::move(h) ) ) );
  }

  template<typename Handler>
  Handler post_wrap_(Handler h)
  {
    return std::move(
      super::owner_wrap_(
        std::move(
          _strand.wrap(
            std::move(
              super::owner_wrap_( std::move(h) )
            )
          )
        )
      )
    );
  }
  
private:
  strand_type _strand;
};

}
