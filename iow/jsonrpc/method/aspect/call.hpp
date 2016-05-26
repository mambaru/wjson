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
    using namespace std::placeholders;
    std::function<data_ptr(const char* name, params_ptr)> ns = std::bind( TT::template serialize_notify<T, params_json>, _1, _2);
    std::function<data_ptr(const char* name, params_ptr, call_id_t )> rs = std::bind( TT::template serialize_request<T, params_json>, _1, _2, _3);
    std::function<void(incoming_holder holder)> rh = t.wrap( [callback](incoming_holder holder)
    { 
      TT::template process_response<T, result_json, error_json>( std::move(holder), std::move(callback) ); 
    });
    
    t.perform_send( 
      tt.name(), 
      std::move(req),
      std::move(ns),
      std::move(rs),
      std::move(rh)
    );
  }

private:

};

}} // iow


