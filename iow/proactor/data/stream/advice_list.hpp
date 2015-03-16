#pragma once

#include <iow/proactor/aspect/tags.hpp>
#include <iow/proactor/data/stream/ad_async_read_some.hpp>

namespace iow{ namespace proactor{ namespace data{ namespace stream{
  
typedef fas::type_list_n<
  fas::advice< _some_, ad_async_read_some >
>::type advice_list;  


}}}}

