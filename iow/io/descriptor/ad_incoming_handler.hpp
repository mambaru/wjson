#pragma once

#include <iow/io/descriptor/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace descriptor{ 

struct ad_incoming_handler
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    const auto& cntx = t.get_aspect().template get<_context_>();
    if ( cntx.incoming_handler != nullptr )
    {
      cntx.incoming_handler(std::move(d), t.get_id_(t), cntx.outgoing_handler);
    }
    else
    {
      t.get_aspect().template get<_outgoing_>()( t, std::move(d) );
    }
  }
};
  
}}}
