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

  
struct handler_base_options
{
  typedef handler_types::send_request_t send_request_t;
  typedef handler_types::send_notify_t send_notify_t;
  
  send_request_t send_request = nullptr;
  send_notify_t  send_notify = nullptr;  
};
  
template<typename T, typename P>
struct handler_options: handler_base_options
{
  T target; 
  P provider;
};



/*
struct ihandler_base
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
};


template<typename T = ihandler_base >
struct ihandler_t: T
{
};
  
struct ihandler: ihandler_t<ihandler_base>
{
  typedef ihandler_t<ihandler_base> super;
  typedef super::outgoing_handler_t outgoing_handler_t;
  typedef super::holder_type holder_type;
  typedef super::io_id_t io_id_t;
  typedef super::call_id_t call_id_t;
  
  virtual ~ihandler() {}
  
  virtual std::shared_ptr<ihandler> clone( ) const= 0;
  
  virtual void invoke(holder_type holder, outgoing_handler_t outgoing_handler) = 0;
  
  virtual std::vector<std::string> get_methods() const = 0;
  
  virtual void start(io_id_t io_id) = 0;
  
  virtual void stop(io_id_t io_id) = 0;

  typedef std::function< data_ptr(const char* name, call_id_t id) > request_serializer_t;
  typedef std::function< data_ptr(const char* name) > notify_serializer_t;
  typedef std::function< void(incoming_holder) > result_handler_t;
  
  typedef std::function< void(const char* name, result_handler_t, request_serializer_t) > send_request_t;
  typedef std::function< void(const char* name, notify_serializer_t) > send_notify_t;
  
  send_request_t send_request = nullptr;
  send_notify_t  send_notify = nullptr;  
};
*/


}} // iow


