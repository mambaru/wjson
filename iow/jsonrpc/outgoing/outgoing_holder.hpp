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
  /*
  typedef std::chrono::high_resolution_clock clock_t;
  typedef clock_t::time_point time_point_t;
  */
  
  typedef ::iow::jsonrpc::data_type data_type;
  typedef ::iow::jsonrpc::data_ptr  data_ptr;
  typedef ::iow::jsonrpc::call_id_t call_id_t;
  typedef std::function< void(incoming_holder) > result_handler_t;

  typedef std::function< data_ptr() > basic_serializer_t;
  // typedef std::function< data_ptr(call_id_t) > request_serializer_t;
  typedef std::function< data_ptr(const char* name, call_id_t id) > request_serializer_t;
  typedef std::function< data_ptr(const char* name) > notify_serializer_t;


  const char* name() const
  {
    return _name;
  }

  outgoing_holder()
    : _name(nullptr)
    , _data(nullptr)
    , _call_id(0)
  {
  }
  
  // полностью сериализованный result, error
  outgoing_holder(data_ptr d)
    : _name(nullptr)
    , _data(std::move(d))
    , _call_id(0)
  {
  }

  // полностью сериализованный notify
  outgoing_holder(const char* name, data_ptr d)
    : _name(name)
    , _data(std::move(d))
    , _call_id(0)
  {
  }

  // частично сериализованный request
  outgoing_holder(const char* name, data_ptr d, result_handler_t result_handler, call_id_t call_id)
    : _name(name)
    , _data(std::move(d))
    , _result_handler(std::move(result_handler))
    , _call_id(call_id)
  {
  }

  // отложенная сериализация result или error
  outgoing_holder(basic_serializer_t serializer)
    : _name(nullptr)
    , _basic_serializer(serializer)
    , _call_id(0)
  {
  }

  // отложенная сериализация исходящих уведомлений
  outgoing_holder(const char* name, notify_serializer_t serializer)
    : _name(name)
    , _notify_serializer(serializer)
    , _call_id(0)
  {
  }

  // отложенная сериализация исходящих запросов
  outgoing_holder(const char* name, request_serializer_t serializer, result_handler_t result_handler, call_id_t call_id)
    : _name(name)
    , _request_serializer(serializer)
    , _result_handler(result_handler)
    , _call_id(call_id)
  {
  }

  bool is_result() const  { return _result_handler==nullptr && _name==nullptr;}
  bool is_request() const { return _result_handler!=nullptr && _name!=nullptr;}
  bool is_notify() const  { return _result_handler==nullptr && _name!=nullptr;}
  call_id_t call_id() const { return _call_id;}

  data_ptr detach()
  {
    if ( _data == nullptr )
    {
      // Отложенная сериализация
      if ( _request_serializer != nullptr )
      {
        return std::move( _request_serializer(_name, _call_id)  );
      }
      else if ( _notify_serializer != nullptr )
      {
        return std::move( _notify_serializer(_name)  );
      }
      else if ( _basic_serializer != nullptr )
      {
        return std::move( _basic_serializer()  );
      }
    }
    else 
    {
      // Параметры или результат уже сериализованны
      if ( this->is_request() )
      {
        auto result = std::make_unique<data_type>();
        result->reserve(_data->size() + 50);
        outgoing_request<data_type> request;
        outgoing_request_json< ::iow::json::raw_value<data_type> >::serializer serializer;
        request.method = _name;
        request.id = _call_id;
        request.params = std::move(_data);
        serializer(request, std::inserter(*result, result->end()));
        return std::move(result);
      }
    }
    // в остальных случаях в _data полностью сериализованый объект или запрос 
    return std::move(_data);
  }

  /*
  data_ptr detach()
  {
    if ( _data!=nullptr )
      return std::move(_data);

    if ( this->is_notify() || this->is_result() )
    {
      if ( _notify_serializer != nullptr )
      {
        return _notify_serializer( _name );
      }
      else if ( _basic_serializer!=nullptr )
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
        return std::move( _request_serializer(_name, call_id)  );
      }
      else if ( _notify_serializer != nullptr )
      {
        return std::move( _notify_serializer(_name)  );
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
  */

  result_handler_t result_handler() const { return _result_handler;}
  void result_handler(result_handler_t handler) { _result_handler=handler;}

private:
  const char* _name;
  data_ptr _data;
  basic_serializer_t _basic_serializer;
  request_serializer_t _request_serializer;
  notify_serializer_t _notify_serializer;
  result_handler_t _result_handler;
  call_id_t _call_id/*_time_point*/;
};

}}
