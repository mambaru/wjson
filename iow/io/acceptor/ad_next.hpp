#pragma once

#include <iow/io/descriptor/tags.hpp>

namespace iow{ namespace io{ namespace acceptor{

struct ad_next
{
  template<typename T>
  struct get_type
  {
    typedef typename T::aspect::template advice_cast< _context_ >::type context_type;
    typedef typename context_type::connection_type connection_type;
    typedef typename context_type::connection_ptr connection_ptr;
    typedef typename connection_type::descriptor_type descriptor_type;
  };
  
  template<typename T>
  auto operator()(T& t)
    -> typename get_type<T>::connection_ptr
  {
    return t.get_aspect().template get<_context_>().manager->create( t.descriptor().get_io_service() );
  }
};
  
}}}
