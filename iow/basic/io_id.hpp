#pragma once

#include <atomic>

namespace iow{ 

typedef size_t io_id_t;

inline io_id_t create_id()
{
  static std::atomic<io_id_t> counter(0);
  return ++counter;
}

}

