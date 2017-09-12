//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2017
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <algorithm>

inline std::string operator "" _json(const char* str, size_t size)
{
  std::string ss( str, str + size);
  std::replace( ss.begin(), ss.end(), '\'', '\"');
  return ss;
}
