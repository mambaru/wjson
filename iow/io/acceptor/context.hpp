#pragma once

#include <iow/io/descriptor/manager.hpp>
#include <memory>

namespace iow{ namespace io{ namespace acceptor{

template<typename ConnectionType>
struct context
{
  typedef ConnectionType connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  typedef ::iow::io::descriptor::manager<connection_type> manager_type;
  typedef std::shared_ptr<manager_type> manager_ptr;
  
  manager_ptr manager;
};
  
}}}
