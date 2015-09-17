#pragma once

#include <memory>

#include <fas/aop.hpp>
#include <fas/integral/bool_.hpp>
#include <iow/logger/logger.hpp>
#include <thread>

namespace iow{ namespace io{ namespace descriptor{ 

// Универсальный для всех
template<typename TgInitialize, bool MakeOutgoingHandler>
struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, O&& opt) 
  {
    typedef typename T::aspect::template advice_cast< _context_>::type context_type;
    context_type& cntx = t.get_aspect().template get<_context_>();

    cntx.outgoing_handler  = opt.outgoing_handler;
    cntx.incoming_handler  = opt.incoming_handler;
    cntx.startup_handler   = opt.startup_handler;
    cntx.shutdown_handler  = opt.shutdown_handler;
    cntx.fatal_handler     = opt.fatal_handler;

    if (  opt.incoming_handler != nullptr )
    {
      this->make_outgoing_(t, cntx, fas::bool_<MakeOutgoingHandler>() );
    }

    if (  cntx.fatal_handler == nullptr )
    {
      cntx.fatal_handler = [](int code, std::string message)
      {
        IOW_LOG_FATAL("iow::io::descriptor: Default fatal handler (" << code << ") " << message)
        abort();
      };
    }

    iow::asio::socket_base::non_blocking_io non_blocking_io(opt.nonblocking);
    t.descriptor().io_control(non_blocking_io);

    t.get_aspect().template get< TgInitialize >()( t, std::forward<O>(opt) );
  }

private:

  template<typename T, typename Cntx> 
  void make_outgoing_(T& t, Cntx& cntx, fas::true_ )
  {
    typedef Cntx context_type;
    auto callback = cntx.outgoing_handler;
    std::weak_ptr<T> wthis = t.shared_from_this();
    cntx.outgoing_handler = t.wrap([wthis, callback](typename context_type::data_ptr d)
    {
      if ( auto pthis = wthis.lock() )
      {
        if ( callback != nullptr )
        {
          callback(std::move(d));
        }
        else
        {
          std::lock_guard<typename T::mutex_type> lk( pthis->mutex() );
          pthis->get_aspect().template get<_outgoing_>()( *pthis, std::move(d) );
        }
      }
    });
  }

  template<typename T, typename Cntx>
  void make_outgoing_(T& /*t*/, Cntx& /*cntx*/, fas::false_ )
  {
  }

};

  
}}}
