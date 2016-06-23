#include <utility>

namespace iow{ namespace json{

/// /////////////////////////////////////////////////////

template<typename T>
struct raw_value
{
  typedef T target;
  typedef serializerT< raw_value< T > > serializer;
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
template<typename K, typename V>
struct pair
{
  typedef typename K::target key_target;
  typedef typename V::target value_target;
  typedef std::pair<key_target, value_target> target;
  typedef serializerT< pair<K, V> > serializer;
  typedef typename K::serializer key_serializer;
  typedef typename V::serializer value_serializer;
};

template<typename T, typename V, typename M, M V::* m, typename W >
struct member_value
{
  typedef T target;
  typedef serializerT< member_value<T, V, M, m, W> > serializer;
};

}}

