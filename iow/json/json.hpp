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

template< typename T, typename L>
struct set_enumerator;

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

/*template<typename C, typename R>
struct array_reserve;
*/
/*
template<typename K, typename V, int R = -1>
struct object2array;
*/

/*
template<typename C, typename R = fas::empty_type>
struct array_r;
*/

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
template<typename L, typename R, bool RU = false >
struct member_if;

template<typename J>
class serializerT;

}}

#include "specialization/json_parser.hpp"
#include "specialization/json_number.hpp"
#include "specialization/json_string.hpp"
#include "specialization/json_object.hpp"
#include "specialization/json_array.hpp"
#include "specialization/json_enum.hpp"
#include "specialization/json_set_enum.hpp"
#include "specialization/json_quoted.hpp"

#include "specialization/value.hpp"
#include "specialization/array.hpp"
#include "specialization/enum.hpp"
#include "specialization/object.hpp"
#include "specialization/dict.hpp"
#include "specialization/utility.hpp"
