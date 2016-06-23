
#include <fas/type_list.hpp>
#include <fas/integral/int_.hpp>
#include <fas/typemanip/empty_type.hpp>

#include <stdexcept>
#include <vector>
#include <array>
#include <string>
#include <deque>
#include <set>
#include <map>
#include <memory>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <unordered_map>
#endif

#include <stdint.h>

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

template< typename T, typename L>
struct set_enumerator
{
  typedef T target;
  typedef typename fas::normalize<L>::type enum_list;
  typedef serializerT< set_enumerator<T, enum_list> > serializer;
};

/// /////////////////////////////////////////////////////////////////

}}

