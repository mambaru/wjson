#pragma once

#include <iow/io/reader/stream/tags.hpp>
#include <utility>
#include <memory>

namespace iow{ namespace io{ namespace reader{ namespace stream{
  
//template<typename DataType>
struct ad_next
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
    return std::move(p);
    //return t.get_aspect().template get<_read_buffer_>().next();
  }
};


}}}}
