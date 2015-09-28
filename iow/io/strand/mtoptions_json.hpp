#pragma once

#include <iow/io/strand/options_json.hpp>
#include <iow/io/strand/mtoptions.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace iow{ namespace io{ namespace strand{

struct mtoptions_json
{
  JSON_NAME(strands)
  JSON_NAME(threads)

  typedef ::iow::json::object<
    mtoptions,
    ::iow::json::member_list<
      ::iow::json::base<options_json>,
      ::iow::json::member<n_strands, mtoptions, size_t, &mtoptions::strands>,
      ::iow::json::member<n_threads, mtoptions, size_t, &mtoptions::threads>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}}
