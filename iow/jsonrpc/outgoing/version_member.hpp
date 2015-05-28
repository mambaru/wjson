#pragma once

#include <iow/jsonrpc/outgoing/outgoing.hpp>
#include <iow/json/json.hpp>
#include <iow/json/name.hpp>

namespace iow{ namespace jsonrpc{

struct version_member
{
  JSON_NAME(jsonrpc)
  typedef ::iow::json::member<n_jsonrpc, outgoing, outgoing::version_type, &outgoing::version> type;
};  

}}

