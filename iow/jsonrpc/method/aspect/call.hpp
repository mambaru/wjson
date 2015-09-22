#pragma once

#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>


#include <iow/jsonrpc/method/aspect/tags.hpp>
#include <iow/jsonrpc/errors/errors.hpp>
#include <iow/jsonrpc/errors/error_json.hpp>
#include <iow/logger/logger.hpp>

#include <fas/aop/metalist.hpp>
#include <functional>
#include <memory>

namespace iow{ namespace jsonrpc{
  
template<typename JParams, typename JResult, typename JError = error_json>
struct call
{
  typedef fas::metalist::advice metatype;
  typedef _call_ tag;
  typedef call<JParams, JResult> advice_class;

  advice_class& get_advice() { return *this;}
  const advice_class& get_advice() const { return *this;}
  
  typedef JParams params_json;
  typedef JResult result_json;
  typedef JError  error_json;
  typedef typename params_json::target params_type;
  typedef typename result_json::target result_type;
  typedef typename error_json::target  error_type;
  
  typedef typename std::unique_ptr<params_type> params_ptr;
  typedef typename std::unique_ptr<result_type> result_ptr;
  typedef typename std::unique_ptr<error_type>  error_ptr;
  
  template<typename T, typename TT>
  void operator()(
    T& t, 
    TT& tt, 
    params_ptr req, 
    std::function< void (result_ptr, error_ptr)>&& callback
  ) const
  {
    JSONRPC_LOG_DEBUG("iow::jsonrpc::call(advice) -1- " )

    if ( callback!=nullptr )
      this->request_(t, tt, std::move(req), std::move(callback) );
    else
      this->notify_(t, tt, std::move(req));

    JSONRPC_LOG_DEBUG("iow::jsonrpc::call(advice) -2- " )

  }

private:
  
  template<typename T, typename TT>
  void request_(
    T& t, 
    TT& tt, // method
    params_ptr req, 
    std::function< void (result_ptr, error_ptr)>&& callback
  ) const
  {
    using namespace std::placeholders;

    std::function<void(incoming_holder holder)> result_handler = nullptr;
    
    if ( callback!=nullptr )
    {
      result_handler = std::bind(
        TT::template process_response<T, result_json, error_json>,
        _1,
        callback 
      );
    }
    
    // name, req, id
    auto ser = std::bind( TT::template serialize_request<T, params_json>, _1, _2, _3);

    JSONRPC_LOG_DEBUG("iow::jsonrpc::call(advice)::request_ -1- " )

    t.send_request( 
      tt.name(), 
      std::move(req),
      std::move(ser),
      std::move(result_handler)
    );

    JSONRPC_LOG_DEBUG("iow::jsonrpc::call(advice)::request_ -2- " )
    
  }
  
  template<typename T, typename TT>
  void notify_(
    T& t, 
    TT& tt, 
    params_ptr req
  ) const
  {
    using namespace std::placeholders;

    t.send_notify( 
      tt.name(), 
      std::move(req),
      std::bind( TT::template serialize_notify<T, params_json>, _1, _2)
    );
   
  }
};

}} // iow


