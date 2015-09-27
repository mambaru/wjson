#pragma once

#include <iow/io/basic/tags.hpp>
#include <iow/io/io_id.hpp>

namespace iow{ namespace io{ namespace basic{

struct ad_create_id
{
  template<typename T>
  void operator()(T& t)
  {
    typedef typename T::aspect::template advice_cast<_context_>::type context_type;
    typedef typename context_type::io_id_type io_id_type;
    t.get_aspect().template get<_context_>().io_id = create_id<io_id_type>();
  }
};

}}}
