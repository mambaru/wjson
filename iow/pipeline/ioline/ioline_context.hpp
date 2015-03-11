#pragma once

#include <iow/pipeline/pipeline_context.hpp>

namespace iow{ namespace ioline{
  
template<typename ConnType >
struct context
  : public ::iow::pipeline::context< ConnType, std::shared_ptr<ConnType>  >
{
  using ::iow::pipeline::context< ConnType, std::shared_ptr<ConnType>  >::reset;
};

}}
