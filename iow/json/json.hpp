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

template<typename T>
struct value;

template<typename C, int R = -1>
struct array;

template<typename T, typename L>
struct object;

template<typename N, typename T, typename M, M T::* m, typename W = value<M> >
struct member;

template<typename J>
struct base;

template< typename T, typename L>
struct enumerator;

template< typename T, typename L>
struct set_enumerator;

template<typename N, typename T, T v>
struct enum_value;

/// 

template<typename T = std::string>
struct raw_value;

template<typename T>
struct value_quoted;

template<typename C, typename R>
struct array_reserve;

template<typename K, typename V, int R = -1>
struct object2array;


template<typename C, typename R = fas::empty_type>
struct array_r;

template<int Reserve>
struct vector_of_strings;

template<typename K, typename V>
struct pair;

template<typename T, typename V, typename M, M V::* m, typename W = value<M> >
struct member_value;


template<typename N,
         typename T,
         typename M,
         typename G, // getter
         typename W = value<M>
        >
struct member_p;

#ifdef __GXX_EXPERIMENTAL_CXX0X__
template<typename ...Args>
using member_list = typename fas::type_list_n<Args...>::type;
#endif

//typedef array< std::vector< binary > > array_of_binary;

/** Ахтунг! замороченые правила:
  * L и M типа member с одинаковыми N (именами)
  * сериализация:
  *   если значение мембера L не равно значению по умолчанию, то сериализуется L, в противном случае R
  * десериализация:
  *   всегда десереализуется в R, а L игнорируется
  * Используется для чего-чего?:
  *   L мембер типа raw_value (т.е. строка голого json как есть) и если он не пустой, то просто копируем в поток эту строку,
  *   если L пустой, то применяем сериализацию R. Соответственно если попытатся десериализовать, то сразу десериализуется по правилам в R.
  *   но если RU задать как false, то копируем в raw_value значение мембера
  */
template<typename L, typename R, bool RU = true >
struct member_if;

template<typename J>
class serializerT;



/// //////////////////////////////////////////////////////// 

/// /////////////////////////////////////////////////////

template<typename J>
struct base: J::member_list {};


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

/// //////////////////////////////////////////////////////////////////////////////

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

template<typename T, typename L>
struct object
{
  typedef T target;
  typedef serializerT< object<T, L> > serializer;
  typedef typename fas::normalize<L>::type member_list;
};

///
template<typename T, typename V, typename M, M V::* m, typename W >
struct member_value
{
  typedef T target;
  typedef serializerT< member_value<T, V, M, m, W> > serializer;
};

/// //////////////////////////////////////////////////////////////////////////////

template<typename N, typename T, typename M, M T::* m, typename W >
struct member: N
{
  typedef T target;
  typedef M type;
  typedef W wrapper;
  typedef typename W::serializer serializer;
  typedef typename W::target wrapper_target;
  const char* operator()() const { return N::operator()(); }

  wrapper_target* ptr(T* t) { return static_cast<wrapper_target*>(t->*m);};
  const wrapper_target* ptr(const T* t) const { return static_cast< const wrapper_target*>(t->*m);};
  wrapper_target& ref(T& t) { return static_cast<wrapper_target&>(t.*m); };
  const wrapper_target& ref(const T& t) const { return static_cast< const wrapper_target&>(t.*m);};
};


template<typename T, typename M, M T::* m>
struct property
{
  void operator()(T& t, const M& value ) const
  {
    t.*m = value;
  }

  const M& operator()(const T& t) const
  {
    return t.*m;
  }
};

template<typename N,
         typename T,
         typename M,
         typename G, // getter
         typename W
        >
struct member_p: N
{
private:
  G _g;
public:
  typedef M type;
  typedef typename W::serializer serializer;
  typedef typename W::target wrapper_target;
  const char* operator()() const { return N::operator()(); }
  void set(T& t, const wrapper_target& v) const { _g(t, v); }
  wrapper_target get(const T& t) const { return _g(t); }
};


template<typename L, typename R, bool RU >
struct member_if
{
};

/// //////////////////////////////////////////////////////////////////////////////

}}

#include "specialization/value.hpp"
#include "specialization/array.hpp"

#include "specialization/json_error.hpp"
#include "specialization/json_parser.hpp"
#include "specialization/json_number.hpp"
#include "specialization/json_string.hpp"
#include "specialization/json_object.hpp"
#include "specialization/json_array.hpp"
#include "specialization/json_enum.hpp"
#include "specialization/json_set_enum.hpp"
#include "specialization/json_quoted.hpp"


