#pragma once

#include <iow/descriptor/descriptor_context.hpp>
#include <iow/aux/buffer_pool.hpp>
#include <memory>
#include <functional>

namespace iow{ namespace proactor{ namespace data{
  
template<typename DataType >
struct context
  : public ::iow::descriptor_context<DataType>
{
  buffer_pool<DataType> incoming_data;
  
  void reset()
  {
    ::iow::descriptor_context<DataType>::reset();
  }
};

}}}