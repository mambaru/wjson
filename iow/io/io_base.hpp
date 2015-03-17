#pragma once

#include <iow/io/basic/tags.hpp>
#include <fas/aop.hpp>
#include <mutex>

// Удалить
#include <iow/owner/owner_handler.hpp>

namespace iow{ namespace io{
  
template<typename A >
class io_base
  : public ::fas::aspect_class<A>
{
public:
  typedef io_base<A> self;
  typedef ::fas::aspect_class<A> super;
  
  typedef typename super::aspect::template advice_cast<_io_service_type_>::type io_service_type;
  typedef typename super::aspect::template advice_cast<_mutex_type_>::type mutex_type;
  typedef typename super::aspect::template advice_cast<_io_id_>::type io_id_t;
  
  ~io_base() {}
  
  io_base(io_service_type& io_service )
    : super()
    , _io_service(io_service)
  {
  }

  io_service_type& get_io_service()
  {
    return _io_service;
  }

  const io_service_type& get_io_service() const
  {
    return _io_service;
  }

  io_id_t get_id() const 
  { 
    std::lock_guard< mutex_type > lk(_mutex);
    return this->get_id_(*this);
  }

  bool status() const 
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->status_(*this);
  }

///  //////////////////////////////////////////////

  void reset()
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->reset_(*this);
  }

  template<typename Handler>
  void post(Handler h)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->post_( std::move(h) );
  }

  template<typename Tg, typename ...Args>
  struct result_of
  {
    typedef typename super::aspect::template advice_cast<Tg>::type advice_type;
    typedef typename std::result_of<advice_type(self&, Args...) >::type type;
  };
  
  
  template<typename Handler>
  auto owner_wrap(Handler h)
    -> typename result_of<_owner_wrap_, Handler>::type
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return std::move( this->owner_wrap_( *this, std::move(h) ) );
  }

  template<typename Handler>
  auto post_wrap(Handler h)
    -> owner_handler< typename std::remove_reference<Handler>::type >
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return std::move( this->post_wrap_( std::move(h) ) );
  }

  mutex_type& mutex() const
  {
    return _mutex;
  }
  
public:
  

  
  template<typename T>
  bool status_(T& t) const
  {
    return t.get_aspect().template get<_status_>();
  }

  template<typename T>
  void reset_(T& t)
  {
    t.get_aspect().template get<_reset_>()(t);
  }

  template<typename T, typename Handler>
  void post_(T& t, Handler h)
  {
    t.get_aspect().template get<_post_>();
    //_io_service.post( std::move( this->owner_wrap_( std::move(h) ) ) );
  }

  template<typename T, typename Handler>
  //typename handler<Handler>::type
  auto owner_wrap_(T& t, Handler h) 
    -> typename result_of<_owner_wrap_, Handler>::type
    //-> owner_handler< typename std::remove_reference<Handler>::type >
    //-> decltype(t.get_aspect().template get<_owner_wrap_>(t, std::ref(h) ) )
  {
    return std::move(t.get_aspect().template get<_owner_wrap_>()(t, std::move(h) ));
    
    //return std::move( _owner.wrap(std::move(h)) );
    
    //return t.get_aspect().template get<_owner_wrap_>(t, std::ref(h) );
    //return std::move( _owner.wrap(std::move(h)) );
  }

  template<typename Handler>
  auto post_wrap_(Handler h)
    -> owner_handler< typename std::remove_reference<Handler>::type >
  {
    return std::move(
      this->owner_wrap_(
        std::move(
          _io_service.wrap(
            std::move(
              this->owner_wrap_( std::move(h) )
            )
          )
        )
      ) 
    );
  }
  
protected:
  
  /*
  template<typename T>
  void reset_(T& )
  {
    _owner.reset();
  }
  */

  template<typename T>
  void start_(T&)
  {
    //_owner.reset();
  }
  
  template<typename T>
  void stop_(T&)
  {
    //_owner.reset();
  }
  
private:
  io_service_type& _io_service;
  //owner _owner;
  mutable mutex_type _mutex;
};

}}
