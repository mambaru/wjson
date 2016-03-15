#pragma once 

#include <iow/json/json.hpp>
#include <iow/workflow/queue_options.hpp>

namespace iow {
  
struct queue_options_json
{
  JSON_NAME(maxsize)
  JSON_NAME(wrnsize)
 
  typedef json::object<
    queue_options,
    json::member_list<
      json::member< n_wrnsize, queue_options, size_t, &queue_options::wrnsize >,
      json::member< n_maxsize, queue_options, size_t, &queue_options::maxsize >
    >
  > type;
  typedef typename type::target target;
  typedef typename type::serializer serializer;
  typedef typename type::member_list member_list;
};

}