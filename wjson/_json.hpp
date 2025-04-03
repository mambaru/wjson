//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2017
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#if __cplusplus >= 201103L
#include <string>
#include <algorithm>
namespace wjson{ namespace literals{
  inline std::string operator ""_json(const char* str, size_t size)
  {
    std::string res(str, size);
    std::replace( res.begin(), res.end(), '\'', '"'); 
    return res;
  }
}}
#endif
