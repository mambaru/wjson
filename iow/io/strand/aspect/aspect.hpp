#pragma once

#include <iow/io/strand/aspect/tags.hpp>
#include <iow/io/strand/aspect/context.hpp>
#include <iow/io/strand/aspect/ad_initialize.hpp>
#include <iow/io/strand/aspect/ad_wrap.hpp>
#include <iow/io/strand/aspect/ad_post.hpp>
#include <iow/io/strand/aspect/ad_size.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace strand{

struct aspect: public fas::aspect
<
  fas::advice< _context_, context >,
  fas::advice< ::iow::io::_initialize_, ad_initialize >,
  fas::advice< _wrap_, ad_wrap >,
  fas::advice< _post_, ad_post >,
  fas::advice< _size_, ad_size >
>{};

}}} // iow::io::strand
