#pragma once

#include <iow/basic/tags.hpp>
#include <fas/aop.hpp>

namespace iow{ 
  
template<typename A = fas::aspect<> >
class io_base
  : public ::fas::aspect_class<A>
{
  
public:
  
  typedef io_base<A> self;
  typedef ::fas::aspect_class<A> super;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::shared_ptr<io_service_type> io_service_ptr;
  
// typedef typename super::aspect::template advice_cast<_owner_type_>::type owner_type;
  typedef typename super::aspect::template advice_cast<_options_type_>::type options_type;
  typedef typename super::aspect::template advice_cast<_mutex_type_>::type mutex_type;
  
  ~io_base()
  {
  }
  
  io_base(io_service_type& io_service, const options_type& opt )
    : _io_service_ptr(nullptr)
    , _io_service(io_service)
    , _options(opt)
  {
    _id = create_id();
  }

  io_base(const options_type& opt )
    : _io_service_ptr( std::make_shared<io_service>() )
    , _io_service( *_io_service_ptr, opt)
    , _options(opt)
  {
    _id = create_id();
  }

  io_id_t get_id() const { return _id;}
  
  io_service_type& get_io_service()
  {
    return _io_service;
  }

  const io_service_type& get_io_service() const
  {
    return _io_service;
  }
  
  const options_type& options() const
  {
    return _options;
  }

  
///  //////////////////////////////////////////////

  template<typename Handler>
  void dispatch(Handler h)
  {
    this->dispatch(*this, h);
  }

  template<typename Handler>
  void post(Handler h)
  {
    this->post(*this, h);
  }
  
  std::function<void()> super_wrap(std::function<void()> fun, std::function<void()> not_alive = nullptr)
  {
    return this->owner().wrap(
      this->strand().wrap(
        this->owner().wrap(
          fun,
          not_alive
        )
      ),
      not_alive
    );
  }
  
  mutex_type& mutex() const
  {
    return _mutex;
  }

  
protected:

  template<typename T, typename Handler>
  void dispatch(T& t, Handler h)
  {
    this->get_aspect().template get<_dispatch_>()(t, h);
  }

  template<typename T, typename Handler>
  void post(T& t, Handler h)
  {
    this->get_aspect().template get<_post_>()(t, h);
  }

  template<typename T>
  void create(T& t)
  {
    t.get_aspect().template gete<_on_create_>()(t);
  }

  template<typename T>
  void start(T& t)
  {
    _stop_flag.clear();
    
    t.get_aspect().template gete<_pre_start_>()(t);
    
    auto& sh = _options.startup_handler;
    
    if ( sh != nullptr )
    {
      auto outgoing_handler = _options.outgoing_handler;
      auto pthis = t.shared_from_this();
      this->mutex().unlock();
      sh(
        this->_id, 
        outgoing_handler,
        //this->options().outgoing_handler,
        [pthis]( std::function<void(io_id_t id)> release_fun ) 
        {
          lock_guard lk(pthis->mutex());
          pthis->_release_handlers2.push_back(release_fun);
          /*t.dispatch( [this, release_fun]()
          {
            this->_release_handlers2.push_back(release_fun);
          });
          */
        }
      );
      this->mutex().lock();
    }
    // Сначала запускаем startup_handler (иначе в mt данные могут прийти раньше )
    t.get_aspect().template gete<_on_start_>()(t);
  }
  
  template<typename T>
  void self_stop(T& t, std::function<void()> finalize)
  {
    if ( !_stop_flag.test_and_set() )
    {
      t.get_aspect().template getg<_on_stop_>()(t);
      auto handlers = std::move(this->_release_handlers2);
      auto id = this->_id;
      _mutex.unlock();
      for ( auto& h : handlers)
        h( id );
      
      if ( finalize!=nullptr )
      {
        finalize();
      }
      _mutex.lock();
    }
  }
  
  template<typename T>
  void stop(T& t, std::function<void()> finalize)
  {
    if ( !_stop_flag.test_and_set() )
    {
      t.get_aspect().template gete<_on_stop_>()(t);

      if ( finalize!=nullptr )
      {
        _mutex.unlock();
        finalize();
        _mutex.lock();
      }
    }
  }
  
private:
  io_service_ptr _io_service_ptr;
  io_service_type& _io_service;

  io_id_t _id;
  options_type _options;
  mutable mutex_type _mutex;
};

}
