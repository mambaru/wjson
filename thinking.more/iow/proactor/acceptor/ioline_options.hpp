#pragma once

#include <iow/proactor/pipeline_context.hpp>

namespace iow{
  
template<typename ConnOptions >
struct ioline_options
{
  typedef ConnOptions descriptor_options;
  // Опции для создаваемого дескриптора (например ip::tcp::connection)
  descriptor_options descriptor;
};

}
