#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <iow/jsonrpc/handler/handler_options.hpp>
#include <iow/io/io_base.hpp>
#include <fas/aop.hpp>
#include <functional>

namespace iow{ namespace jsonrpc{

template< 
  typename A = fas::aspect<>
>
class method_list_base
  : public ::iow::io::io_base<A>
{
public:
  typedef ::iow::io::io_base<A> super;
  typedef method_list_base<A> self;
  //typedef AspectClass<A> super;
  
  typedef typename super::aspect::template advice_cast<_handler_types_>
                        ::type  handler_types;
  typedef typename super::aspect::template advice_cast<_target_>
                        ::type  target_type;
  typedef typename super::aspect::template advice_cast<_provider_>
                        ::type  provider_type;
  typedef typename super::aspect::template advice_cast<_interface_>
                        ::type  interface_type;
  typedef typename super::aspect::template advice_cast<_context_>
                        ::type  context_type;
  
  typedef typename handler_types::call_id_t        call_id_t;
  typedef typename handler_types::io_id_t          io_id_t;
  typedef typename handler_types::holder_type      holder_type;
  typedef typename handler_types::data_type        data_type;
  typedef typename handler_types::data_ptr         data_ptr;
  typedef typename handler_types::error_type       error_type;
  typedef typename handler_types::error_json       error_json;
  typedef typename handler_types::error_ptr        error_ptr;
  
  typedef typename handler_types::outgoing_handler_t    outgoing_handler_t;
  typedef typename handler_types::request_serializer_t  request_serializer_t;
  typedef typename handler_types::result_handler_t      result_handler_t;
  typedef typename handler_types::notify_serializer_t   notify_serializer_t;
  
  typedef handler_options<target_type, provider_type> options_type;
  
  
  template<typename Params, typename Serializer>
  void send_request( 
    const char* name,
    Params params,
    Serializer ser,
    result_handler_t  result_handler) const
  {
    
    super::get_aspect().template get<_send_request_>()(
      //static_cast<const handler_types&>(*this), 
      *this,
      name, 
      std::move(params),
      std::move(ser),
      std::move(result_handler) 
    );
  }
  
  void send_request( const char* name, result_handler_t handler, request_serializer_t ser) const
  {
    this->_send_request( name, std::move(handler), std::move(ser) );
  }
  
  template<typename Params, typename Serializer>
  void send_notify( const char* name, Params params, Serializer ser) const
  {
    super::get_aspect().template get<_send_notify_>()(
      // static_cast<const handler_types&>(*this), 
      *this,
      name, 
      std::move(params),
      std::move(ser)
    );
  }

  void send_notify( const char* name, notify_serializer_t ser) const
  {
    this->_send_notify( name, std::move(ser) );
  }

  template<typename Tg>
  struct call_params_ptr
  {
    typedef typename super::aspect::template advice_cast<Tg>::type
                          ::aspect::template advice_cast<_call_>::type
                          ::params_ptr type;
  };

  template<typename Tg>
  struct call_result_ptr
  {
    typedef typename super::aspect::template advice_cast<Tg>::type
                          ::aspect::template advice_cast<_call_>::type
                          ::result_ptr type;
  };
  
  template<typename Tg>
  struct call_error_ptr
  {
    typedef typename super::aspect::template advice_cast<Tg>::type
                          ::aspect::template advice_cast<_call_>::type
                          ::error_ptr type;
  };

  template<typename Tg, typename ReqPtr>
  void call(
    ReqPtr req, 
    std::function< void(
      typename call_result_ptr<Tg>::type, 
      typename call_error_ptr<Tg>::type
    )> callback
  ) const
  {
    this->get_aspect().template get<Tg>()
                      .call( *this, std::move(req), std::move(callback) );
  }
  
  template<typename Tg, typename ReqPtr>
  void call(
    ReqPtr req, 
    std::function<void(typename call_result_ptr<Tg>::type)> result_callback,
    std::function<void(typename call_error_ptr<Tg>::type)>  error_callback
  ) const
  {
    typedef std::function<void(
      typename call_result_ptr<Tg>::type, 
      typename call_error_ptr<Tg>::type
    )> callback_type;
    
    using namespace std::placeholders;
    callback_type callback = nullptr;
    if ( result_callback!=nullptr )
    {
      callback = std::bind( self::response_handler<Tg>, _1, _2, 
                            result_callback, error_callback);
    }
    
    this->get_aspect().template get<Tg>().call( 
      *this, 
      std::move(req),
      std::move(callback)
    );
  }

private:
  
  template<typename Tg>
  static inline void response_handler(
    typename call_result_ptr<Tg>::type res,
    typename call_error_ptr<Tg>::type err,
    std::function<void(typename call_result_ptr<Tg>::type)> result_callback,
    std::function<void(typename call_error_ptr<Tg>::type)> error_callback
  )
  {
    self::aspect::template advice_cast<_response_handler_>::type
        ::template process<Tg>( 
            std::move(res),
            std::move(err),
            std::move(result_callback),
            std::move(error_callback) 
        );
  }
  
private:
  friend struct super::aspect::template advice_cast< ::iow::io::_initialize_ >::type;
  typedef typename handler_types::send_request_t  send_request_t;
  typedef typename handler_types::send_notify_t   send_notify_t;
  send_request_t _send_request /* = nullptr*/;
  send_notify_t  _send_notify /* = nullptr*/;  

};

}} // iow


