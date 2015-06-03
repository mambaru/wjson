#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <functional>

namespace iow{ namespace jsonrpc{

template< 
  typename A = fas::aspect<>, 
  template<typename> class AspectClass = fas::aspect_class 
>
class method_list_base
  : public AspectClass<A>
  , public AspectClass<A>::aspect::template advice_cast<_ihandler_>::type
{
public:
  typedef method_list_base<A, AspectClass> self;
  typedef AspectClass<A> super;
  
  typedef typename super::aspect::template advice_cast<_ihandler_>
                        ::type  handler_interface;
  typedef typename super::aspect::template advice_cast<_target_>
                        ::type  target_type;
  typedef typename super::aspect::template advice_cast<_provider_>
                        ::type  provider_type;
  typedef typename super::aspect::template advice_cast<_interface_>
                        ::type  interface_type;
  typedef typename super::aspect::template advice_cast<_context_>
                        ::type  context_type;
  
  typedef typename handler_interface::data_type data_type;
  typedef typename handler_interface::data_ptr  data_ptr;
  typedef typename handler_interface::result_handler_t result_handler_t;
  typedef typename handler_interface::error_type error_type;
  typedef typename handler_interface::error_json error_json;
  typedef typename handler_interface::error_ptr  error_ptr;
  
  
  template<typename Params, typename Serializer>
  void send_request( 
    const char* name,
    Params params,
    Serializer ser,
    result_handler_t  result_handler) const
  {
    super::get_aspect().template get<_send_request_>()(
      static_cast<const handler_interface&>(*this), 
      name, 
      std::move(params),
      std::move(ser),
      std::move(result_handler) 
    );
  }
  
  template<typename Params, typename Serializer>
  void send_notify( const char* name, Params params, Serializer ser) const
  {
    super::get_aspect().template get<_send_notify_>()(
      static_cast<const handler_interface&>(*this), 
      name, 
      std::move(params),
      std::move(ser)
    );
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
};

}} // iow


