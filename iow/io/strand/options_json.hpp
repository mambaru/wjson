#pragma once

#include <iow/io/strand/options.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace iow{ namespace io{ namespace strand{

struct options_json
{
  JSON_NAME(disabled)
  JSON_NAME(maxsize)
  JSON_NAME(wrnsize)

  typedef ::iow::json::object<
    options,
    ::wfc::json::member_list<
      ::wfc::json::member<n_disabled, options, bool,   &options::disabled>, 
      ::wfc::json::member<n_maxsize,  options, size_t, &options::maxsize>,
      ::wfc::json::member<n_wrnsize,  options, size_t, &options::wrnsize>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}
