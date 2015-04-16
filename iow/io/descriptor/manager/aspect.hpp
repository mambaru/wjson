#pragma once
#include <iow/io/descriptor/manager/tags.hpp>
#include <iow/io/descriptor/tags.hpp>
#include <iow/io/descriptor/ad_initialize.hpp>
#include <fas/aop.hpp>
#include <memory>
namespace iow{ namespace io{ namespace descriptor{ namespace manager{

template<typename DescriptorHolder>
struct context
{
  typedef DescriptorHolder holder_type;
  std::shared_ptr<holder_type> holder_ptr;
};

struct advice_initialize
  : fas::advice< 
      ::iow::io::descriptor::_initialize_, 
      ::iow::io::descriptor::ad_initialize< 
        false, 
        ::iow::io::descriptor::manager::_initialize_
      >
  >
{};


struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O&& opt) 
  {
  }
};

template<typename DescriptorHolder>
struct aspect: fas::aspect<
  advice_initialize,
  fas::advice<_initialize_, ad_initialize>
>{};

}}}}
