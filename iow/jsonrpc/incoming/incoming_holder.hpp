#pragma once

#include <iow/io/types.hpp>
#include <iow/jsonrpc/incoming/incoming.hpp>
#include <iow/jsonrpc/incoming/incoming_json.hpp>
#include <iow/memory.hpp>

#include <chrono>
#include <cstring>

#define IOW_JSON_EMPTY_OBJECT_STRING "\"~~iow-empty-object~~\""

namespace iow{ namespace jsonrpc{

class incoming_holder
{
public:
  typedef ::iow::io::data_type data_type;
  typedef ::iow::io::data_ptr  data_ptr;
  typedef data_type::iterator  iterator;
  typedef std::pair< iterator, iterator> raw_t;
  typedef std::chrono::high_resolution_clock clock_t;

  incoming_holder(data_ptr d );

  data_ptr parse();

  operator bool () const{ return ready_();}
  bool empty() const { return !ready_();}
  
  bool has_method() const { return ready_() && _incoming.method.first!=_incoming.method.second; }
  bool has_result() const { return ready_() && _incoming.result.first!=_incoming.result.second; }
  bool has_error() const  { return ready_() && _incoming.error.first!=_incoming.error.second;   }
  bool has_id() const     { return ready_() && _incoming.id.first!=_incoming.id.second;         }
  bool has_params() const { return ready_() && _incoming.params.first!=_incoming.params.second; }
  
  bool is_request() const       { return this->has_method() && this->has_id();   }
  bool is_response() const      { return this->has_result() && this->has_id();   }
  bool is_notify() const        { return this->has_method() && !this->has_id();  }
  bool is_error() const         { return this->has_error();                      }
  bool is_request_error() const { return this->has_error() && this->has_id() && 'n'!=*_incoming.params.first;}
  bool is_other_error() const   { return this->has_error() && !this->has_id();   }
  
  bool is_valid() const 
  {
    return this->is_request()
      || this->is_response()
      || this->is_notify()
      || this->is_error();
  }
  
  bool method_equal_to(const char* name) const;
  
  std::string method() const;
  
  raw_t raw_method() const;

  raw_t raw_id() const;
  
  template<typename V, typename J = json::value<V> >
  V get_id() const
  {
    V id = V();
    if ( ready_() )
      typename J::serializer()( id, _incoming.id.first, _incoming.id.second );
    return id;
  }
  
  template<typename J>
  std::unique_ptr<typename J::target> get_result() const
  {
    if ( !this->has_result() )
      return nullptr;
    if ( 'n'==*_incoming.result.first)
      return nullptr; // is null 
    auto result = std::make_unique<typename J::target>();
    if ( '"'==_incoming.result.first[0] && '~'==_incoming.result.first[1])
    {
      if ( 0 == std::strncmp( &(_incoming.result.first[0]), IOW_JSON_EMPTY_OBJECT_STRING, std::strlen(IOW_JSON_EMPTY_OBJECT_STRING) ) )
        return std::move(result);
    }

    typename J::serializer()(*result, _incoming.result.first, _incoming.result.second);
    return std::move(result);
  }
  
  template<typename J>
  std::unique_ptr<typename J::target> get_params() const
  {
    if ( !this->has_params() )
      return nullptr;
    if ( 'n'==*_incoming.params.first)
      return nullptr; // is null 
    auto result = std::make_unique<typename J::target>();
    typename J::serializer()(*result, _incoming.params.first, _incoming.params.second);
    return std::move(result);
  }
  
  template<typename J>
  std::unique_ptr<typename J::target> get_error() const
  {
    if ( !this->has_error() )
      return nullptr;
    if ( 'n'==*_incoming.error.first)
      return nullptr; // is null 
    auto result = std::make_unique<typename J::target>();
    typename J::serializer()(*result, _incoming.error.first, _incoming.error.second);
    return std::move(result);
  }

  std::string error_message(const json::json_error& e) const;
  
  std::string params_error_message(const json::json_error& e) const;

  std::string result_error_message(const json::json_error& e) const;

  std::string error_error_message(const json::json_error& e) const;
  
  std::string id_error_message(const json::json_error& e) const;

  const incoming& get()  const ;
  
  clock_t::time_point time_point() const ;
  
  data_ptr detach();
  
  data_ptr acquire_params();

private:

  bool ready_() const{ return _data != nullptr && _parsed;}

private:
  bool     _parsed;
  data_ptr _data;
  incoming _incoming;
  iterator _begin;
  iterator _end;
  
  clock_t::time_point _time_point;
};
  
}}
