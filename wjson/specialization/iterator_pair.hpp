//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/iterator_pair.hpp>

namespace wjson{

template<typename T>
struct iterator_pair
{
  typedef T target;
  typedef serializerT< iterator_pair< T > > serializer;
};

}

