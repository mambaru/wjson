//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/serializer/iterator_pair.hpp>

namespace iow{ namespace json{

template<typename T>
struct iterator_pair
{
  typedef T target;
  typedef serializerT< iterator_pair< T > > serializer;
};

}}

