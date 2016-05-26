#pragma once

#include <iow/jsonrpc/outgoing/outgoing_request.hpp>
#include <iow/jsonrpc/outgoing/outgoing_request_json.hpp>
#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/basic_types.hpp>
#include <iow/memory.hpp>

#include <chrono>
#include <cstring>

namespace iow{ namespace jsonrpc{

class outgoing_holder
{
public:
  typedef ::iow::jsonrpc::data_type data_type;
  typedef ::iow::jsonrpc::data_ptr  data_ptr;
  typedef ::iow::jsonrpc::call_id_t call_id_t;
  typedef std::function< void(incoming_holder) > result_handler_t;
  typedef std::function< data_ptr() > basic_serializer_t;
  typedef std::function< data_ptr(const char* name, call_id_t id) > request_serializer_t;
  typedef std::function< data_ptr(const char* name) > notify_serializer_t;

  outgoing_holder();
  
  // полностью сериализованный result, error
  outgoing_holder(data_ptr d);

  // полностью сериализованный notify
  outgoing_holder(const char* name, data_ptr d);

  // частично сериализованный request
  outgoing_holder(const char* name, data_ptr d, result_handler_t result_handler, call_id_t call_id);

  // отложенная сериализация result или error
  outgoing_holder(basic_serializer_t serializer);

  // отложенная сериализация исходящих запросов
  outgoing_holder(const char* name, notify_serializer_t ns, request_serializer_t rs, result_handler_t rh, call_id_t call_id);

  data_ptr detach();
  outgoing_holder clone() const;
  outgoing_holder clone(call_id_t call_id) const;

  bool is_result() const  { return _result_handler==nullptr && _name==nullptr;}
  bool is_request() const { return _result_handler!=nullptr && _name!=nullptr;}
  bool is_notify() const  { return _result_handler==nullptr && _name!=nullptr;}
 
  const char* name() const;
  call_id_t call_id() const { return _call_id;}
  result_handler_t result_handler() const;
  void result_handler(result_handler_t handler);

private:

  const char* _name;
  data_ptr _data;
  basic_serializer_t _basic_serializer;
  notify_serializer_t _notify_serializer;
  request_serializer_t _request_serializer;
  result_handler_t _result_handler;
  call_id_t _call_id;
};

}}
