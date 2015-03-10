#pragma once

#include <cstddef>
#include <atomic>
#include <iow/basic/types.hpp>

namespace iow{ 

inline io_id_t create_id()
{
  static std::atomic<io_id_t> counter(0);
  return ++counter;
}

}

