#pragma once

#include <cstddef>
#include <atomic>

namespace iow{ namespace io{

template<typename T>
inline T create_id()
{
  static std::atomic<T> counter( (T()) );
  return counter.fetch_add(1);
}

}}

