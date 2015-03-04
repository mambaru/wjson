#pragma once

#include <iow/basic/callback_owner.hpp>
#include <iow/basic/io_id.hpp>
#include <iow/basic/tags.hpp>
#include <iow/aux/asio.hpp>


#include <fas/aop.hpp>

#include <thread>
#include <mutex>

#define IOW_INHERITING_CONSTRUCTORS_T(self, super)                                                        \
  self( typename super::io_service_type& io_service, const typename super::options_type& opt )            \
    : super(io_service, opt)                                                                              \
  {}                                                                                                      \
  /*self(const typename super::options_type& opt ) : super(opt){}*/                                           \
  /*self( typename super::io_service_type& io_service): super(io_service) {}*/                                \
  /*self(): super(){}*/                                                                                       \
  
#define IOW_INHERITING_CONSTRUCTORS(self, super)                                                          \
  self( super::io_service_type& io_service, const super::options_type& opt )                              \
    : super(io_service, opt)                                                                              \
  {}                                                                                                      \
  /*self(const super::options_type& opt ) : super(opt){}                   */                                 \
  /*self( super::io_service_type& io_service): super(io_service) {}        */                                 \
  /*self(): super(){}                                                      */                             \
  
  
#define IOW_IO_BASE_IMPL_T(super)                                                                         \
  void start() {                                                                                          \
    std::lock_guard< mutex_type > lk( super::mutex() );                                                   \
    this->get_aspect().template gete< _before_start_ >()(*this);                                          \
    super::start_(*this);                                                                                 \
    this->get_aspect().template gete< _after_start_ >()(*this);                                           \
  }                                                                                                       \
  void stop() {                                                                                           \
    std::lock_guard< mutex_type > lk( super::mutex() );                                                   \
    this->get_aspect().template gete< _before_stop_ >()(*this);                                           \
    super::stop_(*this);                                                                                  \
    this->get_aspect().template gete< _after_stop_ >()(*this);                                            \
  }                                                                                                       \
  void reset(const options_type& opt) {                                                                   \
    std::lock_guard< mutex_type > lk( super::mutex() );                                                   \
    super::reset_(*this, opt);                                                                            \
    this->get_aspect().template gete< _on_reset_ >()(*this);                                              \
  }                                                                                                       \

  
namespace iow{ 
  
template<typename A >
class io_base
  : public ::fas::aspect_class<A>
{
public:
  typedef io_base<A> self;
  typedef ::fas::aspect_class<A> super;
  typedef ::iow::asio::io_service io_service_type;
  
  typedef typename super::aspect::template advice_cast<_options_type_>::type options_type;
  typedef typename super::aspect::template advice_cast<_context_type_>::type context_type;
  typedef typename super::aspect::template advice_cast<_mutex_type_>::type mutex_type;
  
  ~io_base()
  {
  }
  
  io_base(io_service_type& io_service, const options_type& opt = options_type() )
    : _io_service(io_service)
    , _options(opt)
    , _status(false)
  {
    _id = ::iow::create_id();
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

  options_type options() const
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->options_();
  }

  bool status() const 
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return this->status_();
  }
  
  void cancel()
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->cancel_();
  }

///  //////////////////////////////////////////////

  template<typename Handler>
  void post(Handler h)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    this->post_( std::move(h) );
  }

  template<typename Handler>
  Handler owner_wrap(Handler h)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return std::move( this->owner_wrap_( std::move(h) ) );
  }

  template<typename Handler>
  Handler post_wrap(Handler h)
  {
    std::lock_guard< mutex_type > lk(_mutex);
    return std::move( this->post_wrap_( std::move(h) ) );
  }

  mutex_type& mutex() const
  {
    return _mutex;
  }
  
public:
  
  // not thread safe
  const options_type& options_() const
  {
    return _options;
  }

  const context_type& context_() const
  {
    return _context;
  }

  context_type& context_()
  {
    return _context;
  }

  bool status_() const
  {
    return _status;
  }

  void cancel_()
  {
    _status = false;
  }

  template<typename Handler>
  void post_(Handler h)
  {
    _io_service.post( std::move( this->owner_wrap_( std::move(h) ) ) );
  }

  template<typename Handler>
  auto owner_wrap_(Handler h) 
    -> callback_wrapper< typename std::remove_reference<Handler>::type >
  {
    return std::move( _owner.wrap(std::move(h)) );
  }

  template<typename Handler>
  Handler post_wrap_(Handler h)
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
  
  template<typename T>
  void reset_(T&, const options_type& opt )
  {
    _owner.reset();
    _options = opt;
    _status = false;
    _id = ::iow::create_id();
    _context.reset();
  }

  template<typename T>
  void start_(T&)
  {
    _status = true;
  }
  
  template<typename T>
  void stop_(T&)
  {
    _owner.reset();
    _status = false;
  }
  
private:
  callback_owner _owner;
  io_service_type& _io_service;
  options_type _options;
  bool _status;
  io_id_t _id;
  mutable mutex_type _mutex;
  context_type _context;
};

}
