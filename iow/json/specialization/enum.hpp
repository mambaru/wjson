#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/serializer/enum.hpp>
#include <fas/type_list/normalize.hpp>

namespace iow{ namespace json{

template<typename N, typename T, T v>
struct enum_value: N
{
  static const T value = v;
  const char* operator()() const { return N::operator()(); }
};

template< typename T, typename L>
struct enumerator
{
  typedef T target;
  typedef typename fas::normalize<L>::type enum_list;
  typedef serializerT< enumerator<T, enum_list> > serializer;
  typedef enum_list member_list;
};

/// //////////////////////////////////////////////////////////////////////////////

template< typename T, typename L, char Sep>
struct flags_enumerator
{
  typedef T target;
  typedef typename fas::normalize<L>::type enum_list;
  typedef serializerT< flags_enumerator<T, enum_list, Sep> > serializer;
  typedef enum_list member_list;
};

/// /////////////////////////////////////////////////////////////////

}}

