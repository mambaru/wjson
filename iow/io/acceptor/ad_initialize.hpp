#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace acceptor{

struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, const O& opt)
  {
    //t.get_aspect().template get<_context_>().connection_options = opt.connection;
    
    typedef typename T::aspect::template advice_cast< _context_ >::type context_type;
    typedef typename context_type::io_id_type io_id_type;
    context_type& context = t.get_aspect().template get<_context_>();

    auto conn_opt = opt.connection;
    auto sh = conn_opt.shutdown_handler;
    std::weak_ptr<T> wthis = t.shared_from_this();
    conn_opt.shutdown_handler = t.wrap([wthis, sh](io_id_type id)
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_context_>().manager->erase(id);
      }
      
      if ( sh!=nullptr )
      {
        sh(id);
      }
    });
    context.manager = std::make_shared<typename context_type::manager_type>( std::move(conn_opt) );
    context.addr = opt.addr;
    context.port = opt.port;
    context.backlog = opt.backlog;
    
  }
};

}}}
