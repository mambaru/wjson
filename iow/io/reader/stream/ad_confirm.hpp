#pragma once

#include <iow/io/reader/stream/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace reader{ namespace stream{

struct ad_rollback
{
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    bool result = t.get_aspect().template get<_read_buffer_>().rollback( std::move(p) );
    if ( !result )
    {
      // read error
      // t.get_aspect().template get<_error_>(t, "")
    }
    }
};

struct ad_confirm
{
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    bool result = t.get_aspect().template get<_read_buffer_>().confirm( std::move(p) );
    if ( !result )
    {
      // read error
      // t.get_aspect().template get<_error_>(t, "")
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
