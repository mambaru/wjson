#pragma once

#include <iow/io/stream/tags.hpp>
#include <utility>

namespace iow{ namespace io{ namespace stream{
  
//template<typename DataType>
struct ad_read_next
{
  template<typename T>
  std::pair<char*, size_t> operator()(T& t)
  {
    // Проверить размер 
    
    auto p = t.get_aspect().template get<_read_buffer_>().next();
    if ( p.first == nullptr )
    {
      // TODO: жесткое закрытие 
    }
    return t.get_aspect().template get<_read_buffer_>().next();
  }
};


}}}
