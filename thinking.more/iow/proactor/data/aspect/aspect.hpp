#pragma once 

//#include <iow/proactor/aspect/aspect_pipeline.hpp>
#include <iow/proactor/data/aspect/ad_write_more.hpp>
#include <iow/proactor/data/aspect/ad_write_ready.hpp>
#include <iow/proactor/data/aspect/ad_make_data.hpp>
#include <iow/proactor/data/aspect/ad_read_handler.hpp>
#include <iow/proactor/data/aspect/ad_read_error.hpp>
#include <iow/proactor/data/aspect/tags.hpp>

#include <iow/proactor/aspect/tags.hpp>
#include <fas/aop.hpp>


namespace iow{ namespace proactor{ namespace data{

typedef fas::type_list_n<
  fas::advice<_make_data_, ad_make_incoming_data>,
  fas::advice<_prepare_data_, ad_prepare_data>,
  fas::advice<_write_more_, ad_write_more>,
  fas::advice<_write_ready_, ad_write_ready>,
  fas::advice<_read_handler_, ad_read_handler>,
  fas::advice<_read_error_, ad_read_error>
>::type advice_list;
  
typedef fas::aspect<
  advice_list
> aspect;
  
}}}
