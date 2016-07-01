//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//
#pragma once

#include <fas/type_list.hpp>
#include <string>

namespace iow{ namespace json{

/// /////////////////////////////////////////////////////

template<typename T, int R = -1>
struct value;

template<typename T, int R = -1>
struct array;

template<typename T, typename L>
struct object;

template<typename N, typename T, typename M, M T::* m, typename W = value<M> >
struct member;

template<typename T, typename L>
struct object_array;

template<typename T, typename M, M T::* m, typename W = value<M> >
struct member_array;

template<typename J>
struct base;

template< typename T, typename L>
struct enumerator;

template< typename T, typename L, char Sep = ','>
struct flags_enumerator;

template<typename N, typename T, T v>
struct enum_value;

template<typename T, int R = -1>
struct dict;

template<typename VJ, int R = 10>
struct dict_vector;

template<typename VJ, int R = 10>
struct dict_deque;

template<typename VJ>
struct dict_map;

/// 

template<typename T = std::string, int R = -1>
struct raw_value;

template<typename Itr>
struct iterator_pair;

template<typename J, bool SerQ = true, bool ReqQ = true>
struct raw_quoted;

template<typename J, bool SerQ = true, bool ReqQ = true, int R = -1>
struct quoted;

template<typename T, typename J>
struct pointer;

template<int R1=-1, int R2=-1>
struct vector_of_strings;

template<int R1=-1, int R2=-1>
struct deque_of_strings;

template<int R1=-1, int R2=-1>
struct list_of_strings;

template<typename K, typename V>
struct field;

template<typename T, typename V, typename M, M V::* m, typename W = value<M> >
struct member_value;

template<typename N,
         typename T,
         typename M,
         typename G, // getter
         typename W = value<M>
        >
struct member_p;

#if __cplusplus >= 201103L
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
template<typename L, typename R, bool RU = false >
struct member_if;

template<typename J>
class serializerT;

}}
