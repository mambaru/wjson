#pragma once

#include <iow/io/writer/stream/tags.hpp>
#include <utility>


namespace iow{ namespace io{ namespace writer{ namespace stream{
  

struct ad_confirm
{
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    bool result = t.get_aspect().template get<_write_buffer_>().confirm(p);
    if ( !result )
    {
      // Все закртыть, и выдать в log
    }
    /*
    auto d = t.get_aspect().template get<_write_buffer_>().confirm(p);
    if ( d != nullptr )
    {
      if ( t.get_aspect().template get<_buffer_pool_>()!=nullptr )
      {
        t.get_aspect().template get<_buffer_pool_>()->free( std::move(d) );
      }
    }
    */
  }
};

}}}}
