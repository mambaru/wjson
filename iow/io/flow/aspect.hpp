#pragma once

#include <iow/io/flow/tags.hpp>
#include <iow/io/flow/ad_start.hpp>
#include <iow/io/flow/ad_more.hpp>
#include <iow/io/flow/ad_complete.hpp>

#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace flow{

template<typename HandlerTag, typename InputType >
struct aspect: fas::aspect<
  fas::advice< _start_, ad_start >,
  fas::advice< _more_, ad_more >,
  fas::advice< _complete_, ad_complete >,
  fas::type< _input_type_, InputType>,
  fas::group< ::iow::io::_after_start_, _start_>,
  fas::alias< _handler_, HandlerTag>
>{};

}}}
