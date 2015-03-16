#pragma once


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
  echo/*,
  jsonrpc,
  http
  */
};

}