#pragma once 

#include <iow/workflow/queue_options_json.hpp>
#include <iow/workflow/workflow_options.hpp>
#include <wfc/json.hpp>

namespace iow {
  
struct workflow_options_json
{
  JSON_NAME(threads)
  /*JSON_NAME(enabled)*/
  JSON_NAME(use_io_service)
 
  typedef json::object<
    workflow_options,
    json::member_list<
      /*json::member< n_enabled, workflow_options, bool, &workflow_options::enabled >,*/
      json::base< queue_options_json >,
      json::member< n_threads, workflow_options, int,  &workflow_options::threads >,
      json::member< n_use_io_service, workflow_options, bool, &workflow_options::use_io_service >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

struct workflow_options2_json
{
  JSON_NAME(post_delay_ms)
  JSON_NAME(rate_limit)

  typedef json::object<
    workflow_options,
    json::member_list<
      json::base< workflow_options_json >,
      json::member< n_post_delay_ms, workflow_options, time_t, &workflow_options::post_delay_ms >,
      json::member< n_rate_limit, workflow_options, size_t, &workflow_options::rate_limit >
    >
  > type;

  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

struct workflow_options_on_json
{
  JSON_NAME(threads)
  JSON_NAME(use_io_service)
 
  typedef json::object<
    workflow_options,
    json::member_list<
      json::base< ::iow::queue_options_json >,
      json::member< n_threads, workflow_options, int,  &workflow_options::threads >,
      json::member< n_use_io_service, workflow_options, bool, &workflow_options::use_io_service >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}
