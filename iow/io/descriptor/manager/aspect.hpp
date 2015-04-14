#pragma once
#include <iow/io/descriptor/manager/tags.hpp>
#include <fas/aop.hpp>
#include <memory>
namespace iow{ namespace io{ namespace descriptor{ namespace manager{

template<typename DescriptorHolder>
struct context
{
  typedef DescriptorHolder holder_type;
  std::shared_ptr<holder_type> holder_ptr;
};


template<typename DescriptorHolder>
struct aspect: fas::aspect<
>{};

}}}}
