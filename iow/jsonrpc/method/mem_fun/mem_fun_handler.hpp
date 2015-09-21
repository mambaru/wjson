#pragma once

#include <iow/jsonrpc/method/mem_fun/mem_fun_helper.hpp>
#include <wfc/logger.hpp>

namespace iow{ namespace jsonrpc{

template<
  typename Params, 
  typename Result, 
  typename I, 
  void (I::*mem_ptr)( 
    std::unique_ptr<Params>, 
    std::function< void(std::unique_ptr<Result>) > 
  ) 
>
struct mem_fun_handler
{
  typedef std::unique_ptr<Params> request_ptr;
  typedef std::unique_ptr<Result> responce_ptr;
  typedef std::unique_ptr< ::iow::jsonrpc::error> json_error_ptr;
  typedef std::function< void(responce_ptr, json_error_ptr) > jsonrpc_callback;

  template<typename T>
  void operator()(T& t, request_ptr req, jsonrpc_callback cb) const
  {
    if ( auto i = t.target() )
    {

      JSONRPC_LOG_DEBUG("mem_fun_handler -1- " << (i.get()!=nullptr) << " and " << typeid(i.get()).name() << ": " << size_t(i.get()) )

      (i.get()->*mem_ptr)( 
        std::move(req), 
        mem_fun_make_callback( std::move(cb)) 
      );
      
      JSONRPC_LOG_DEBUG("mem_fun_handler -2-" )
    }
    else 
    {
      mem_fun_service_unavailable( std::move(cb) );
    }
  }
};

}} // iow


