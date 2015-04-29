#pragma once

#include <iow/owner/owner.hpp>
#include <fas/aop.hpp>
#include <mutex>
#include <functional>

namespace iow{ namespace io{ namespace basic{

template<typename MutexType /*= std::recursive_mutex */>  
struct context
{
  typedef size_t io_id_type;
  typedef bool status_type;
  typedef owner owner_type;
  typedef MutexType mutex_type;
  typedef std::function<void(io_id_type)> shutdown_complete;
  
  status_type status;
  io_id_type  io_id;
  owner_type  holder;
  //mutable mutex_type mutex;
};

}}}
