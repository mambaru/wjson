#pragma once
#include <iow/io/descriptor/manager/tags.hpp>
#include <iow/io/descriptor/tags.hpp>
#include <iow/io/descriptor/ad_initialize.hpp>
#include <fas/aop.hpp>
#include <memory>
namespace iow{ namespace io{ namespace descriptor{ namespace manager{

struct manager_options
{
  
};

template<typename DescriptorHolder>
class manager_impl
{
public:
  typedef DescriptorHolder holder_type;
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef typename holder_type::descriptor_type descriptor_type;
  typedef typename holder_type::options_type options_type;
  typedef typename holder_type::io_id_type io_id_type;
  typedef ::iow::asio::io_service io_service;
  
  manager_impl(io_service& io)
    : _io(io)
  {}
  
  descriptor_type next()
  {
    return descriptor_type(_io);
  }
  
  bool confirm(descriptor_type d)
  {
    return 
  }

  holder_ptr detach()
  {
    return nullptr;
  }
  
  
private:
  typedef std::map< io_id_type, holder_ptr> holder_map;
  io_service _io;
  holder_map _holders;

};

  
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
