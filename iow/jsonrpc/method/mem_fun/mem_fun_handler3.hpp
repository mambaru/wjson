#pragma once

#include <iow/jsonrpc/method/mem_fun/mem_fun_helper.hpp>

namespace iow{ namespace jsonrpc{

template<
  typename Params, 
  typename Result, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<Params>, 
    std::function< void(std::unique_ptr<Result>) >, 
    size_t
  )
>
struct mem_fun_handler3
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
      (i.get()->*mem_ptr)( 
        std::move(req), 
        mem_fun_make_callback( std::move(cb)), 
        t.get_id() 
      );
    }
    else 
    {
      mem_fun_service_unavailable( std::move(cb) );
    }
  }
};

}} // iow


