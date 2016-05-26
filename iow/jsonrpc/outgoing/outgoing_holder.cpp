
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>
#include <iow/jsonrpc/outgoing/outgoing_request_json.hpp>
#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/basic_types.hpp>
#include <iow/memory.hpp>

#include <chrono>
#include <cstring>

namespace iow{ namespace jsonrpc{

const char* outgoing_holder::name() const
{
  return _name;
}

outgoing_holder::outgoing_holder()
  : _name(nullptr)
  , _data(nullptr)
  , _call_id(0)
{}

// полностью сериализованный result, error
outgoing_holder::outgoing_holder(data_ptr d)
  : _name(nullptr)
  , _data(std::move(d))
  , _call_id(0)
{
}

// полностью сериализованный notify
outgoing_holder::outgoing_holder(const char* name, data_ptr d)
  : _name(name)
  , _data(std::move(d))
  , _call_id(0)
{
}

// частично сериализованный request
outgoing_holder::outgoing_holder(const char* name, data_ptr d, result_handler_t result_handler, call_id_t call_id)
  : _name(name)
  , _data(std::move(d))
  , _result_handler(std::move(result_handler))
  , _call_id(call_id)
{
}

// отложенная сериализация result или error
outgoing_holder::outgoing_holder(basic_serializer_t serializer)
  : _name(nullptr)
  , _basic_serializer(serializer)
  , _call_id(0)
{
}

// отложенная сериализация исходящих запросов
outgoing_holder::outgoing_holder(
  const char* name,
  notify_serializer_t ns,
  request_serializer_t rs,
  result_handler_t rh,
  call_id_t call_id
) : _name(name)
  , _notify_serializer(ns)
  , _request_serializer(rs)
  , _result_handler(rh)
  , _call_id(call_id)
{
}

data_ptr outgoing_holder::detach()
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

outgoing_holder::result_handler_t outgoing_holder::result_handler() const 
{
  return _result_handler;
}

void outgoing_holder::result_handler(result_handler_t handler) 
{
  _result_handler=handler;
}

outgoing_holder outgoing_holder::clone() const
{
  outgoing_holder holder;
  holder._name = this->_name;
  holder._data = std::make_unique<data_type>(*(this->_data));
  holder._basic_serializer = this->_basic_serializer;
  holder._request_serializer = this->_request_serializer;
  holder._notify_serializer = this->_notify_serializer;
  holder._result_handler = nullptr;
  holder._call_id = 0;
  return std::move(holder);
}

outgoing_holder outgoing_holder::clone(call_id_t call_id) const
{
  outgoing_holder holder;
  holder._name = this->_name;
  holder._data = std::make_unique<data_type>(*(this->_data));
  holder._basic_serializer = this->_basic_serializer;
  holder._request_serializer = this->_request_serializer;
  holder._notify_serializer = this->_notify_serializer;
  holder._result_handler = this->_result_handler;
  holder._call_id = call_id;
  return std::move(holder);
}

}}
