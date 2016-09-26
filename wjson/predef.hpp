//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <fas/type_list.hpp>
#include <string>

namespace wjson{

template<typename T, int R = -1>
struct value;

template<typename T, int R = -1>
struct array;

template<typename T, typename L>
struct object;

template<typename N, typename T, typename M, M T::* m, typename J = value<M> >
struct member;

template<typename T, typename L>
struct object_array;

template<typename T, typename M, M T::* m, typename J = value<M> >
struct member_array;

template<typename J>
struct base;

template< typename T, typename L>
struct enumerator;

template< typename T, typename L, char Sep = ','>
struct flags;

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

template<typename N, typename T, typename M, typename G, typename W = value<M> >
struct member_p;

#if __cplusplus >= 201103L
template<typename ...Args>
using member_list = typename fas::type_list_n<Args...>::type;
#else
template< typename T1 = fas::empty_list,  typename T2 = fas::empty_list,  typename T3 = fas::empty_list,  
          typename T4 = fas::empty_list,  typename T5 = fas::empty_list,  typename T6 = fas::empty_list,
          typename T7 = fas::empty_list,  typename T8 = fas::empty_list,  typename T9 = fas::empty_list,  
          typename T10 = fas::empty_list, typename T11 = fas::empty_list, typename T12 = fas::empty_list,
          typename T13 = fas::empty_list, typename T14 = fas::empty_list, typename T15 = fas::empty_list, 
          typename T16 = fas::empty_list, typename T17 = fas::empty_list, typename T18 = fas::empty_list,
          typename T19 = fas::empty_list, typename T20 = fas::empty_list, typename T21 = fas::empty_list, 
          typename T22 = fas::empty_list, typename T23 = fas::empty_list, typename T24 = fas::empty_list,
          typename T25 = fas::empty_list, typename T26 = fas::empty_list
>
struct member_list
 : public fas::type_list_n<
    T1,   T2,   T3,   T4,
    T5,   T6,   T7,   T8,
    T9,   T10,  T11,  T12,
    T13,  T14,  T15,  T16,
    T17,  T18,  T19,  T20,
    T21,  T22,  T23,  T24,
    T25,  T26
>::type {};
#endif

template<typename L, typename R, bool RU = false >
struct member_if;

template<typename J>
class serializerT;

}
