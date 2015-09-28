#pragma once

#include <iow/io/strand/aspect/tags.hpp>
#include <memory>
#include <ctime>

namespace iow{ namespace io{ namespace strand{

struct ad_initialize
{
  template<typename T, typename Opt>
  void operator()(T& t, Opt&& opt)
  {
    typedef typename T::aspect::template advice_cast<_context_>::type context_type;
    typedef typename context_type::counter_type counter_type;
    typedef typename context_type::strand_type strand_type;
    context_type& context = t.get_aspect().template get<_context_>();
    context.maxsize = opt.maxsize;
    context.wrnsize = opt.wrnsize;
    context.wrntime = std::time(0);
    context.counter = std::make_shared<counter_type>(0);
    if ( !opt.disabled )
    {
      context.strand = std::make_shared<strand_type>(t.get_io_service());
    }
  }
};

}}} // iow::io::strand
