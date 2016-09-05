//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace wjson{

template<typename N, typename T, T v>
struct enum_value: N
{
  static const T value = v;
  const char* operator()() const { return N::operator()(); }
};

}

