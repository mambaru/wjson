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
  typedef std::chrono::high_resolution_clock clock_t;
  typedef clock_t::time_point time_point_t;
  
  typedef ::iow::jsonrpc::data_type data_type;
  typedef ::iow::jsonrpc::data_ptr  data_ptr;
  typedef ::iow::jsonrpc::call_id_t call_id_t;
  typedef std::function< void(incoming_holder) > result_handler_t;

  typedef std::function< data_ptr() > basic_serializer_t;
  typedef std::function< data_ptr(call_id_t) > request_serializer_t;

  const char* name() const
  {
    return _name;
  }

  outgoing_holder(time_point_t time_point = time_point_t())
    : _name(nullptr)
    , _data(nullptr)
    , _time_point(time_point)
  {
  }
  
  // полностью сериализованный result, error
  outgoing_holder(data_ptr d, time_point_t time_point = time_point_t())
    : _name(nullptr)
    , _data(std::move(d))
    , _time_point(time_point)
  {
  }

  // полностью сериализованный notify
  outgoing_holder(const char* name, data_ptr d, time_point_t time_point = time_point_t())
    : _name(name)
    , _data(std::move(d))
    , _time_point(time_point)
  {
  }

  // частично сериализованный request
  outgoing_holder(const char* name, data_ptr d, result_handler_t result_handler, time_point_t  time_point = time_point_t())
    : _name(name)
    , _data(std::move(d))
    , _result_handler(std::move(result_handler))
    , _time_point(time_point)
  {
  }

  // отложенная сериализация result или error
  outgoing_holder(basic_serializer_t serializer, time_point_t  time_point = time_point_t())
    : _name(nullptr)
    , _basic_serializer(serializer)
    , _time_point(time_point)
  {
  }

  // отложенная сериализация исходящих уведомлений
  outgoing_holder(const char* name, basic_serializer_t serializer, time_point_t  time_point = time_point_t())
    : _name(name)
    , _basic_serializer(serializer)
    , _time_point(time_point)
  {
  }

  // отложенная сериализация исходящих запросов
  outgoing_holder(const char* name, request_serializer_t serializer, result_handler_t result_handler, time_point_t time_point = time_point_t())
    : _name(name)
    , _request_serializer(serializer)
    , _result_handler(result_handler)
    , _time_point(time_point)
  {
  }

  bool is_result() const  { return _result_handler==nullptr;}
  bool is_request() const { return _result_handler!=nullptr && _name!=nullptr;}
  bool is_notify() const  { return _name!=nullptr && _result_handler==nullptr;}

  data_ptr detach()
  {
    if ( _data!=nullptr )
      return std::move(_data);

    if ( this->is_notify() || this->is_result() )
    {
      if ( _basic_serializer!=nullptr )
      {
        return std::move( _basic_serializer() );
      }
    }
    return nullptr;
  }

  data_ptr detach(call_id_t call_id)
  {
    if ( !this->is_request() )
      return nullptr;
    
    if ( _data == nullptr )
    {
      if ( _request_serializer != nullptr )
      {
        return std::move( _request_serializer(call_id)  );
      }
    }
    else
    {
      auto result = std::make_unique<data_type>();
      result->reserve(_data->size() + 50);
      outgoing_request<data_type> request;
      outgoing_request_json< ::iow::json::raw_value<data_type> >::serializer serializer;
      request.method = _name;
      request.id = call_id;
      request.params = std::move(_data);
      serializer(request, std::inserter(*result, result->end()));
      return std::move(result);
    }
    return nullptr;
  }

  result_handler_t result_handler() const { return _result_handler;}
  void result_handler(result_handler_t handler) { _result_handler=handler;}

private:
  const char* _name;
  data_ptr _data;
  basic_serializer_t _basic_serializer;
  request_serializer_t _request_serializer;
  result_handler_t _result_handler;
  time_point_t _time_point;
};

}}
