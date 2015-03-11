#include <fas/aop.hpp>

namespace iow{
  
enum class domain
{
  IP,
  IPv4,
  IPv6,
  local
};

enum class trasport
{
  tcp,
  udp,
  local
};

enum class type
{
  dgram,
  stream
};

enum class layer
{
  bin,
  text,
  jsonrpc,
  http
};

template<domain D, trasport Tr>
struct aspect_protocol;

template<type Ty>
struct aspect_type;

template<layer L>
struct aspect_layer;


template<domain D, trasport Tr, type Ty, layer L, typename A = fas::empty_type>
struct aspect_connection: fas::aspect<
  aspect_protocol<D, Tr>,
  aspect_type<Ty>,
  aspect_layer<Ty>,
  A
>
{};

}
