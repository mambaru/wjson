#pragma once

#include <iow/pipeline/pipeline_context.hpp>

namespace iow{

template<typename DescType >
struct idescriptor_manager
{
  typedef DescType descriptor_type;
  typedef std::shared_ptr<descriptor_type> descriptor_ptr;
  
  virtual descriptor_ptr create()  = 0;
  virtual void start(io_id_t id)  = 0;
  virtual void stop(io_id_t id)  = 0;
  virtual void error(io_id_t id)  = 0;
};



template<typename ConnType >
struct ioline_manager
  : pipeline_context< ConnType, std::shared_ptr<ConnType>  >
{
  using pipeline_context< ConnType, std::shared_ptr<ConnType>  >::reset;
};

}
