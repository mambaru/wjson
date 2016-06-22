#pragma once

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

/// /////////////////////////////////////////////////////

template< typename T, char L, char R >
class serializerA;

template<typename C, typename R>
struct array_base;

template<typename C, typename R>
struct array_reserve
{
  template<typename T>
  inline void reserve(T& t)
  {
    t.reserve(R::value);
  }
};

template<typename C>
struct array_reserve<C, fas::empty_type>
{
  template<typename T>
  inline void reserve(T&)
  {
  }
};

template<typename J, typename R>
struct array_base< std::vector<J>, R>
{
  typedef std::vector<J> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::vector<target> target_container;
  typedef serializerT< array_r< json_container, R> > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

template<typename J, typename R>
struct array_base< std::deque<J>, R>
{
  typedef std::deque<J> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::deque<target> target_container;
  typedef serializerT< array_r< json_container, R> > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

template<typename K, typename V, typename R>
struct array_base< std::vector< pair<K, V> >, R>
{
  typedef pair<K, V> json_value;
  typedef std::vector< json_value > json_container;
  typedef typename json_value::target target;
  typedef std::vector<target> target_container;
  typedef serializerA< array_r< json_container, R>, '{', '}' > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

template<typename J, int N, typename R>
struct array_base< J[N], R>
{
  typedef J json_container[N];
  typedef J json_value;
  typedef typename json_value::target target;
  typedef target target_container[N];
  typedef serializerT< array_r< json_container, R > > serializer;
};

template<typename J, typename R>
struct array_base< std::set<J>, R>
{
  typedef std::set<J> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::set<target> target_container;
  typedef serializerT< array_r< json_container, R > > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};

template<typename J, typename R>
struct array_base< std::multiset<J>, R>
{
  typedef std::multiset<J> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::multiset<target> target_container;
  typedef serializerT< array_r< json_container, R > > serializer;
  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};


#ifdef __GXX_EXPERIMENTAL_CXX0X__
template<typename JK, typename JV, typename R>
struct array_base< std::unordered_map<JK, JV>, R>
{
  typedef std::unordered_map<JK, JV> json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef pair< JK , JV > json_value;
  typedef typename json_value::target target;
  typedef std::unordered_map< key, value > target_container;
  typedef serializerA< array_r< json_container, R >, '{', '}' > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};


template<typename J, size_t N, typename R>
struct array_base< std::array<J, N>, R>
{
  typedef std::array<J, N> json_container;
  typedef J json_value;
  typedef typename json_value::target target;
  typedef std::array<target, N> target_container;
  typedef serializerT< array_r< json_container, R> > serializer;
};

#endif

template<typename JK, typename JV, typename R>
struct array_base< std::map<JK, JV>, R >
{
  typedef std::map<JK, JV> json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef pair< JK , JV > json_value;
  typedef typename json_value::target target;
  typedef std::map< key, value > target_container;
  typedef serializerA< array_r< json_container, R >, '{', '}' > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};


struct n_key { const char* operator()() const{ return "key";} };
struct n_value { const char* operator()() const{ return "value";} };

template<typename JK, typename JV, typename R>
struct array_base< std::multimap<JK, JV>, R>
{
  typedef std::multimap<JK, JV> json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef object<
            pair_type,
            typename fas::type_list_n<
              member< n_key, pair_type, key, &pair_type::first, JK >,
              member< n_value, pair_type, value, &pair_type::second, JV >
            >::type
          > json_value;
  typedef typename json_value::target target;
  typedef std::map< key, value > target_container;

  typedef serializerT< array_r< json_container, R> > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};





template<typename C, typename R>
struct array_r
  : public array_base<C, R>
  , public array_reserve<C, R>
{
  typedef array_base<C, R> base;
  typedef typename base::target_container target;
  typedef typename base::serializer serializer;

};

template< typename T, typename L, typename R>
struct array_r< object<T, L>, R>
  : array_r< std::vector< object<T, L> >, R>
{
};

template< typename K, typename V, typename R>
struct array_r< pair<K, V>, R>
  : array_r< std::vector< pair<K, V> >, R>
{
};

template<typename C, int R>
struct array: array_r<C, fas::int_<R> > {};

template<typename C>
struct array<C, -1>: array_r<C, fas::empty_type> {};

template<typename KJ, typename VJ, int R>
struct object2array: array< pair<KJ,VJ>, R > {};

template<int Reserve>
struct vector_of_strings: array< std::vector< value< std::string > >,  Reserve> {};

}}
