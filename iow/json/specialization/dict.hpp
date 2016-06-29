

#include <fas/type_list.hpp>
#include <fas/integral/int_.hpp>
#include <fas/typemanip/empty_type.hpp>

#include <vector>
#include <array>
#include <string>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <memory>

#if __cplusplus >= 201103L
#include <unordered_map>
#include <unordered_set>
#endif

#include <stdint.h>

namespace iow{ namespace json{

/// /////////////////////////////////////////////////////

template< typename T, char L, char R >
class serializerA;

template<typename C, typename R>
struct dict_base;

template<typename C, typename R>
struct dict_r;


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

template<typename C, typename R>
struct dict_reserve
{
  template<typename T>
  inline void reserve(T& t)
  {
    t.reserve(R::value);
  }
};

template<typename C>
struct dict_reserve<C, fas::empty_type>
{
  template<typename T>
  inline void reserve(T&)
  {
  }
};

template<typename K, typename V, typename R>
struct dict_base< std::vector< pair<K, V> >, R>
{
  typedef pair<K, V> json_value;
  typedef std::vector< json_value > json_container;
  typedef typename json_value::target target;
  typedef std::vector<target> target_container;
  typedef serializerA< dict_r< json_container, R>, '{', '}' > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

template<typename K, typename V, typename R>
struct dict_base< std::deque< pair<K, V> >, R>
{
  typedef pair<K, V> json_value;
  typedef std::deque< json_value > json_container;
  typedef typename json_value::target target;
  typedef std::deque<target> target_container;
  typedef serializerA< dict_r< json_container, R>, '{', '}' > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

template<typename K, typename V, typename R>
struct dict_base< std::list< pair<K, V> >, R>
{
  typedef pair<K, V> json_value;
  typedef std::list< json_value > json_container;
  typedef typename json_value::target target;
  typedef std::list<target> target_container;
  typedef serializerA< dict_r< json_container, R>, '{', '}' > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

template<typename K, typename V, typename R>
struct dict_base< std::set< pair<K, V> >, R>
{
  typedef pair<K, V> json_value;
  typedef std::set< json_value > json_container;
  typedef typename json_value::target target;
  typedef std::set<target> target_container;
  typedef serializerA< dict_r< json_container, R>, '{', '}' > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

template<typename JK, typename JV, typename R>
struct dict_base< std::map<JK, JV>, R >
{
  typedef std::map<JK, JV> json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef pair< JK , JV > json_value;
  typedef typename json_value::target target;
  typedef std::map< key, value > target_container;
  typedef serializerA< dict_r< json_container, R >, '{', '}' > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};

template<typename JK, typename JV, typename R>
struct dict_base< std::multimap<JK, JV>, R >
{
  typedef std::map<JK, JV> json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef pair< JK , JV > json_value;
  typedef typename json_value::target target;
  typedef std::map< key, value > target_container;
  typedef serializerA< dict_r< json_container, R >, '{', '}' > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};

#if __cplusplus >= 201103L

template<typename JK, typename JV, typename R>
struct dict_base< std::unordered_map<JK, JV>, R>
{
  typedef std::unordered_map<JK, JV> json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef pair< JK , JV > json_value;
  typedef typename json_value::target target;
  typedef std::unordered_map< key, value > target_container;
  typedef serializerA< dict_r< json_container, R >, '{', '}' > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};

template<typename JK, typename JV, typename R>
struct dict_base< std::unordered_multimap<JK, JV>, R>
{
  typedef std::unordered_map<JK, JV> json_container;
  typedef typename JK::target key;
  typedef typename JV::target value;
  typedef std::pair<key, value> pair_type;
  typedef pair< JK , JV > json_value;
  typedef typename json_value::target target;
  typedef std::unordered_map< key, value > target_container;
  typedef serializerA< dict_r< json_container, R >, '{', '}' > serializer;

  typedef std::insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::inserter(t, t.begin()); }
};

template<typename K, typename V, typename R>
struct dict_base< std::array< pair<K, V>, R::value >, R>
{
  typedef pair<K, V> json_value;
  typedef std::array< json_value, R::value > json_container;
  typedef typename json_value::target target;
  typedef std::array< target, R::value> target_container;
  typedef serializerA< dict_r< json_container, R>, '{', '}' > serializer;
};

template<typename K, typename V, typename R>
struct dict_base< std::unordered_set< pair<K, V> >, R>
{
  typedef pair<K, V> json_value;
  typedef std::set< json_value > json_container;
  typedef typename json_value::target target;
  typedef std::set<target> target_container;
  typedef serializerA< dict_r< json_container, R>, '{', '}' > serializer;
  typedef std::back_insert_iterator<target_container> inserter_iterator;
  static inserter_iterator inserter(target_container& t) { return std::back_inserter(t); }
};

#endif

template<typename C, typename R>
struct dict_r
  : public dict_base<C, R>
  , public dict_reserve<C, R>
{
  typedef dict_base<C, R> base;
  typedef typename base::target_container target;
  typedef typename base::serializer serializer;

};


template<typename C, int R>
struct dict: dict_r<C, fas::int_<R> > {};

template<typename C>
struct dict<C, -1>: dict_r<C, fas::empty_type> {};

template<typename VJ, int R>
struct dict_vector: dict< std::vector< pair< value<std::string>, VJ > >, R > {};

template<typename VJ, int R>
struct dict_deque: dict< std::deque< pair< value<std::string>,VJ > >, R > {};

template<typename VJ>
struct dict_map: dict< std::map< value<std::string>, VJ >, -1 > {};


}}
