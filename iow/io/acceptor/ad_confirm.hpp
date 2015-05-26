#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace acceptor{

struct ad_confirm
{
  
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    typedef typename T::io_id_type io_id_type;
    typedef typename type_<T>::options_type options_type;
    options_type opt = t.get_aspect().template get<_context_>().connection_options;
    
    opt.incoming_handler = []( 
      typename options_type::data_ptr d, 
      io_id_type, 
      typename options_type::outgoing_handler_t callback
    )
    {
      callback( std::move(d) );
    };
    
    std::weak_ptr<T> wthis = t.shared_from_this();
    opt.shutdown_handler = t.wrap([wthis](io_id_type id)
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_context_>().manager->close(id);
      }
    });

    p->start(opt);
    t.get_aspect().template get<_context_>().manager->attach(p->get_id(), p);
  }

private:
  template<typename T>
  struct type_
  {
    typedef typename T::aspect::template  advice_cast<_context_>::type context_type;
    typedef typename context_type::connection_type connection_type;
    typedef typename connection_type::options_type options_type;
  };

};

  
}}}
