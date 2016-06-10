#pragma once

#include <iow/workflow/thread_pool_base.hpp>

namespace iow {

template<typename Service >
class thread_pool
  : public thread_pool_base
{
  typedef thread_pool_base super;
public:
  typedef thread_pool<Service> self;
  typedef Service service_type;
  typedef std::shared_ptr<service_type> service_ptr;

  thread_pool(service_ptr service)
    : _service(service)
  {
  }
  
  bool reconfigure(int threads)
  {
    return super::reconfigure(_service, threads);
  }
    
  void start(int threads)
  {
    super::start(_service, threads);
  }

private:
  service_ptr _service;
};

}
