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
      t.mutex().unlock();
#warning      try
      {
        incoming( std::move(d), std::move(io_id), std::move(outgoing));
      }
      /*catch(const std::exception& e)
      {
        if ( outgoing ) outgoing(nullptr);
        std::lock_guard<typename T::mutex_type> lk(t.mutex());
        cntx.fatal_handler(-1, std::string("iow::io::descriptor: std::exception: ") + std::string(e.what()));
      }
      catch(...)
      {
        if ( outgoing ) outgoing(nullptr);
        std::lock_guard<typename T::mutex_type> lk(t.mutex());
        cntx.fatal_handler(-1, "iow::io::descriptor: Unhandled exception in incoming handler");
      }*/
      t.mutex().lock();

    }
    else
    {
      t.get_aspect().template get<_outgoing_>()( t, std::move(d) );
    }
  }
};
  
}}}
