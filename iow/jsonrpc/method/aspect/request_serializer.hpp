#pragma once

#include <iow/jsonrpc/method/aspect/tags.hpp>
#include <iow/jsonrpc/outgoing/outgoing_request.hpp>
#include <iow/jsonrpc/outgoing/outgoing_request_json.hpp>
#include <fas/aop.hpp>
#include <iow/memory.hpp>

namespace iow{ namespace jsonrpc{

template< size_t ReserveSize = 80 >
struct request_serializer
  : fas::type<_request_serializer_, request_serializer<ReserveSize> >
{
  template<typename T, typename JParams>
  static inline auto 
  serialize(
    const char* name, 
    std::unique_ptr<typename JParams::target> req, 
    typename T::call_id_t id
  ) -> typename T::data_ptr
  {
    typedef typename JParams::target request_type;
    outgoing_request<request_type> request;
    request.params = std::move(req);
    request.method = name; 
    request.id = id;
    auto d = std::make_unique< typename T::data_type>();
    d->reserve(ReserveSize);
    typedef outgoing_request_json<JParams> send_json;
    typename send_json::serializer()(request, std::inserter( *d, d->end() ));
    return std::move(d);
  }
};

struct request_serializer_proxy
  : fas::type<_request_serializer_, request_serializer_proxy >
{

  template<typename T, typename JParams>
  static inline auto 
  serialize(
    const char* name, 
    std::unique_ptr<typename JParams::target> req, 
    typename T::call_id_t id
  ) -> typename T::data_ptr
  {
    return T::aspect::template advice_cast<_request_serializer_>::type
            ::template serialize<T, JParams>( name, std::move(req), id );

  }
};

}} // iow


