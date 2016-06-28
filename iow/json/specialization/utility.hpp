#include <utility>

namespace iow{ namespace json{

/// /////////////////////////////////////////////////////

template<typename T, int R>
struct raw_value
{
  typedef T target;
  typedef serializerT< raw_value< T, R > > serializer;
};

template<typename T>
struct raw_pair
{
  typedef T target;
  typedef serializerT< raw_pair< T > > serializer;
};

template<typename T, typename J>
struct pointer
{
  typedef T target;
  typedef serializerT< pointer< T, J > > serializer;
};

/// /////////////////////////////////////////

template<typename T, typename V, typename M, M V::* m, typename W >
struct member_value
{
  typedef T target;
  typedef serializerT< member_value<T, V, M, m, W> > serializer;
};

}}

