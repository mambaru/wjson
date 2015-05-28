#pragma once

#include <fas/functional/tstring.hpp>

#define JSONRPC_TAG(x) FAS_STRING( _##x##_, #x)
#define JSONRPC_TAG2(x,y) FAS_STRING( x, y)

