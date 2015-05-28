#pragma once

#include <iow/jsonrpc/errors.hpp>
#include <iow/io/types.hpp>
#include <iow/memory.hpp>
#include <functional>

namespace iow{ namespace jsonrpc{

template<typename Result>
inline auto mem_fun_make_callback( std::function< void(std::unique_ptr<Result>, std::unique_ptr< ::iow::jsonrpc::error>) >&& cb)
  -> std::function<void(std::unique_ptr<Result>) >
{
  if (cb==nullptr)
    return nullptr;
    
  return [cb]( std::unique_ptr<Result> resp)
  {
    cb( std::move(resp), nullptr);
  };
}

template<typename Result>
inline void mem_fun_service_unavailable( std::function< void(std::unique_ptr<Result>, std::unique_ptr< ::iow::jsonrpc::error>) >&& cb)
{
  if (cb!=nullptr)
  {
    cb( nullptr, std::make_unique< ::iow::jsonrpc::error >( ::iow::jsonrpc::service_unavailable() ) );
  }
}

}} // iow


