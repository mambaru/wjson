#pragma once

namespace iow{ namespace jsonrpc{

struct outgoing
{
  typedef char version_type[4];
  version_type version;
  outgoing()
    : version{'2','.','0','\0'}
  {}
};

}}
