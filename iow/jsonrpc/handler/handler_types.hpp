#pragma once

#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/errors/errors.hpp>
#include <iow/jsonrpc/errors/error_json.hpp>
#include <memory>

namespace iow{ namespace jsonrpc{

struct handler_types
{
  typedef incoming_holder holder_type;
  typedef holder_type::data_type data_type;
  typedef holder_type::data_ptr  data_ptr;
  typedef ::iow::io::outgoing_handler_t outgoing_handler_t;
  typedef ::iow::io::io_id_t io_id_t;
  typedef int call_id_t;

  typedef ::iow::jsonrpc::error error_type;
  typedef ::iow::jsonrpc::error_json error_json;
  typedef std::unique_ptr<error_type> error_ptr;

  typedef std::function< data_ptr(const char* name, call_id_t id) > request_serializer_t;
  typedef std::function< data_ptr(const char* name) > notify_serializer_t;
  typedef std::function< void(incoming_holder) > result_handler_t;

  typedef std::function< void(const char* name, result_handler_t, request_serializer_t) > send_request_t;
  typedef std::function< void(const char* name, notify_serializer_t) > send_notify_t;
};

}} // iow


