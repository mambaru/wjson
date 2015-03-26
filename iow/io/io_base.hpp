#pragma once

#include <iow/io/basic/tags.hpp>
#include <fas/aop.hpp>
#include <mutex>

// Удалить
#include <iow/owner/owner_handler.hpp>


namespace iow{ namespace io{
  
/**
 * start()
 * stop()
 * reset()
 * initialize()
 */
template<typename A >
class io_base
  : public ::fas::aspect_class<A>
{
public:
  typedef io_base<A> self;
  typedef ::fas::aspect_class<A> super;
  
  typedef typename super::aspect::template advice_cast<_mutex_type_>::type mutex_type;
  typedef typename super::aspect::template advice_cast<_io_id_>::type io_id_t;
  typedef typename super::aspect::template advice_cast<_status_>::type status_t;

  mutex_type& mutex() const
  {
    return _mutex;
  }
  
  io_id_t get_id() const 
  { 
    std::lock_guard< mutex_type > lk(_mutex);
    return this->get_id_(*this);
  }

  status_t status() const 
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->status_(*this);
  }

  void reset()
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->reset_(*this);
  }

  template<typename O>
  void start(O&& opt)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->start_(*this, std::forward<O>(opt));
  }

  template<typename O>
  void initialize(O&& opt)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->initialize_(*this, std::forward<O>(opt));
  }

  void stop()
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->stop_(*this);
  }

  

  void shutdown()
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->stop_(*this);
  }
  
  template<typename Tg, typename ...Args>
  struct result_of
  {
    typedef typename super::aspect::template advice_cast<Tg>::type advice_type;
    typedef typename std::result_of<advice_type(self&, Args...) >::type type;
  };
  
  template<typename Handler>
  auto wrap(Handler&& h)
    -> typename result_of<_wrap_, Handler>::type
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->wrap_( *this, std::forward<Handler>(h) );
  }

  template<typename Handler, typename AltHandler>
  auto wrap(Handler&& h, AltHandler&& ah)
    -> typename result_of<_wrap_, Handler, AltHandler>::type
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->wrap_( *this, std::forward<Handler>(h), std::forward<AltHandler>(ah) );
  }

public:
  
  template<typename T>
  status_t status_(T& t) const
  {
    return t.get_aspect().template get<_status_>();
  }

  template<typename T>
  io_id_t get_id_(T& t) const
  {
    return t.get_aspect().template get<_io_id_>();
  }

  template<typename T>
  void reset_(T& t)
  {
    t.get_aspect().template get<_reset_>()(t);
  }

  template<typename T, typename O>
  void start_(T& t, O&& opt )
  {
    t.get_aspect().template get<_start_>()(t, std::forward<O>(opt) );
  }

  template<typename T, typename O>
  void initialize_(T& t, O&& opt )
  {
    t.get_aspect().template gete<_initialize_>()(t, std::forward<O>(opt) );
  }

  template<typename T>
  void stop_(T& t)
  {
    t.get_aspect().template get<_stop_>()(t);
  }

  template<typename T, typename Handler>
  void shutdown_(T& t, Handler&& handler)
  {
    t.get_aspect().template get<_shutdown_>()(t, std::forward<Handler>(handler));
  }
  
  

  template<typename T, typename Handler>
  auto wrap_(T& t, Handler&& h) 
    -> typename result_of<_wrap_, Handler>::type
  {
    return t.get_aspect().template get<_wrap_>()(t, std::forward<Handler>(h) );
  }

  template<typename T, typename Handler, typename AltHandler>
  auto wrap_(T& t, Handler&& h, AltHandler&& ah) 
    -> typename result_of<_wrap_, Handler, AltHandler>::type
  {
    return t.get_aspect().template get<_wrap_>()(t, std::forward<Handler>(h), std::forward<AltHandler>(ah) );
  }

private:
  
  mutable mutex_type _mutex;
};

}}
