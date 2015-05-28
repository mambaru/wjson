#pragma once

#include <iow/jsonrpc/method/mem_fun/mem_fun_helper.hpp>
#include <iow/logger/logger.hpp>
namespace iow{ namespace jsonrpc{


template<
  typename Params, 
  typename Result, 
  typename Params2, 
  typename Result2, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<Params>, 
    std::function< void(std::unique_ptr<Result>) >, 
    size_t, 
    std::function< void(
      std::unique_ptr<Params2>, 
      std::function< void(std::unique_ptr<Result2>) >  
    ) >
  ),
  typename Itf,
  void (Itf::*mem_ptr2)( 
    std::unique_ptr<Params2>, 
    std::function< void(std::unique_ptr<Result2>) >
  )
>
struct mem_fun_handler2
{
  typedef std::unique_ptr<Params> request_ptr;
  typedef std::unique_ptr<Result> responce_ptr;
  typedef std::unique_ptr<Params2> request2_ptr;
  typedef std::unique_ptr<Result2> responce2_ptr;
  typedef std::unique_ptr< ::iow::jsonrpc::error> json_error_ptr;
  typedef std::function< void(responce_ptr, json_error_ptr) > jsonrpc_callback;

  template<typename T>
  void operator()(T& t, request_ptr req, jsonrpc_callback cb) const
  {
    if ( auto i = t.target() )
    {
      std::shared_ptr<Itf> pthis = t.shared_from_this();
      (i.get()->*mem_ptr)( 
        std::move(req), 
        mem_fun_make_callback( std::move(cb)),
        t.get_io_id(),
        [pthis, this](request2_ptr req, std::function< void(responce2_ptr) > callback)
        {
          if ( auto ptr = pthis.get() )
          {
            (ptr->*mem_ptr2)( std::move(req), callback);
          }
          else
          {
            JSONRPC_LOG_FATAL("mem_fun_handler2: pthis = nullptr");
            abort();
          }
        }
      );
    }
    else 
    {
      JSONRPC_LOG_ERROR("mem_fun_handler2: Service Unavailable");
      mem_fun_service_unavailable( std::move(cb) );
    }
  }
};

}} // iow


