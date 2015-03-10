#pragma once

#include <iow/pipeline/pipeline_context.hpp>

namespace iow{
  
template<typename ConnType >
struct ioline_context
  : pipeline_context< ConnType, std::shared_ptr<ConnType>  >
{
  using pipeline_context< ConnType, std::shared_ptr<ConnType>  >::reset;
};

}
