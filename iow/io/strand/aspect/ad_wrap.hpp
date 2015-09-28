#pragma once

#include <iow/io/strand/aspect/tags.hpp>
#include <iow/logger/logger.hpp>
#include <iow/io/basic/tags.hpp>
#include <functional>
#include <cassert>
#include <stdexcept>
#include <memory>

namespace iow{ namespace io{ namespace strand{

struct ad_wrap
{
  typedef std::function<void()> strand_wrapper;

  template<typename T, typename F>
  auto operator()(T& t, F&& f)
    -> typename T::template result_of< ::iow::io::_wrap_, strand_wrapper >::type
  {
    auto& context = t.get_aspect().template get<_context_>();
    auto counter = context.counter;
    ++(*counter);
    std::function<void()> ff = [f, counter]()
    {
      assert( *counter > 0 );
      --(*counter);
      try
      {
        f();
      }
      catch(const std::exception& e)
      {
        IOW_LOG_ERROR("iow::io::strand exception: " << e.what())
      }
      catch(...)
      {
        IOW_LOG_ERROR("iow::io::strand unhandled exception")
      }
    };

    return t.basic_wrap_( t, std::move(ff) );
  }
};

}}} // iow::io::strand
