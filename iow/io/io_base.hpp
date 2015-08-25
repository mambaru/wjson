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
private:
  typedef io_base<A> self;
  typedef ::fas::aspect_class<A> super;
  typedef typename super::aspect::template advice_cast< basic::_context_ >::type context_type;
  
public:
  typedef typename context_type::mutex_type mutex_type;
  typedef typename context_type::io_id_type io_id_type;
  typedef typename context_type::status_type status_type;
  typedef typename context_type::shutdown_complete shutdown_complete;
  typedef typename super::aspect::template advice_cast<_options_type_>::type options_type;

  /*
  typedef typename super::aspect::template advice_cast<_mutex_type_>::type mutex_type;
  typedef typename super::aspect::template advice_cast<_io_id_>::type io_id_t;
  typedef typename super::aspect::template advice_cast<_status_>::type status_t;
  typedef typename super::aspect::template advice_cast<_options_type_>::type options_type;
  */
  

  mutex_type& mutex() const
  {
    return _mutex;
  }
  
  io_id_type get_id() const 
  { 
    std::lock_guard< mutex_type > lk(_mutex);
    return this->get_id_(*this);
  }

  status_type status() const 
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
  void reconfigure(O&& opt)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->reconfigure_(*this, std::forward<O>(opt));
  }

  void stop()
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->stop_(*this);
  }

  void shutdown(shutdown_complete&& handler)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->shutdown_(*this, std::forward<shutdown_complete>(handler));
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
  status_type status_(T& t) const
  {
    // TODO: через адвайсы?
    return t.get_aspect().template get< basic::_context_>().status;
  }

  template<typename T>
  io_id_type get_id_(T& t) const
  {
    return t.get_aspect().template get< basic::_context_>().io_id;
    //return t.get_aspect().template get<_io_id_>();
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
  void reconfigure_(T& t, O&& opt )
  {
    //!!! t.get_aspect().template gete<_initialize_>()(t, std::forward<O>(opt) );
    // Определяет пользователь
    t.get_aspect().template get<_initialize_>()(t, std::forward<O>(opt) );
  }

  template<typename T>
  void stop_(T& t)
  {
    t.get_aspect().template get<_stop_>()(t);
  }

  template<typename T>
  void shutdown_(T& t, shutdown_complete&& handler)
  {
    t.get_aspect().template get<_shutdown_>()(t, std::forward<shutdown_complete>(handler));
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
