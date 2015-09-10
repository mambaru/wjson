#pragma once

#include <iow/io/descriptor/tags.hpp>
#include <memory>
#include <iow/logger/logger.hpp>
namespace iow{ namespace io{ namespace descriptor{

struct ad_incoming_handler
{
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    const auto& cntx = t.get_aspect().template get<_context_>();
    if ( cntx.incoming_handler != nullptr )
    {
      auto incoming = cntx.incoming_handler;
      auto outgoing = cntx.outgoing_handler;
      auto io_id = t.get_id_(t);
      //cntx.incoming_handler(std::move(d), t.get_id_(t), cntx.outgoing_handler);
      t.mutex().unlock();
      try
      {
        incoming( std::move(d), io_id, std::move(outgoing));
      }
      catch(...)
      {
        // TODO: в лог и outgoing(nullptr) ???
      }
      t.mutex().lock();

    }
    else
    {
      t.get_aspect().template get<_outgoing_>()( t, std::move(d) );
    }
  }
};
  
}}}
