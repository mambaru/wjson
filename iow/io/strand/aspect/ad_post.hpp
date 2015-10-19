#pragma once

#include <iow/io/strand/aspect/tags.hpp>
#include <iow/logger/logger.hpp>
#include <memory>

namespace iow{ namespace io{ namespace strand{

struct ad_post
{
  template<typename T, typename F>
  bool operator()(T& t,F&& f)
  {
    auto& context = t.get_aspect().template get<_context_>();
    //auto func = t.get_aspect().template get<_wrap_>()(t, std::forward<F>(f) );
    auto func = std::move(f);
    auto counter = context.counter;
    ++(*counter);

    size_t size = t.size_(t);
    if ( context.maxsize!=0 &&  size >= context.maxsize )
    {
      IOW_LOG_ERROR("mt_strand queue overflow size=" << size << " ( maxsize = " << context.maxsize << ")")
      return false;
    }
    else if ( context.wrnsize!=0 && size > context.wrnsize )
    {
      time_t now = time(0);
      if ( now != context.wrntime )
      {
        IOW_LOG_WARNING("mt_strand queue overflow warning size = " << size << "( wrnsize=" << context.wrnsize << ")")
        context.wrntime = now;
      }
    }

    if ( context.strand != nullptr )
    {
//#warning обходим strand в качестве эксперимента 
      //context.strand->get_io_service().post( std::move(func) );
      context.strand->post( std::move(func) );
    }
    else
    {
      t.get_io_service().post( std::move(func)  );
    }
    return true;
  }
};

}}} // iow::io::strand
