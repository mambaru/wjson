#pragma once

#include <iow/jsonrpc/noparams.hpp>
#include <iow/json/json.hpp>

namespace iow{ namespace jsonrpc{

struct noparams_json
{
  typedef ::iow::json::object<
    noparams,
    ::iow::json::member_list<
    >
  > type;

  typedef type::serializer serializer;
  typedef type::target target;
};

}}