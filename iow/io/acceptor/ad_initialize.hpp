#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <memory>

namespace iow{ namespace io{ namespace acceptor{

struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, const O& opt)
  {
    t.get_aspect().template get<_context_>().connection_options = opt.connection;
    
    typedef typename T::aspect::template advice_cast< _context_ >::type context_type;
    context_type& context = t.get_aspect().template get<_context_>();
    context.manager = std::make_shared<typename context_type::manager_type>();
    context.addr = opt.addr;
    context.port = opt.port;
    context.backlog = opt.backlog;
  }
};

}}}
