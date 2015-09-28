#pragma once

#include <iow/io/strand/aspect/tags.hpp>
#include <cstddef>

namespace iow{ namespace io{ namespace strand{

struct ad_size
{
  template<typename T>
  size_t operator()(T& t) const
  {
    auto& context = t.get_aspect().template get<_context_>();
    return *(context.counter);
  }
};

}}} // iow::io::strand
