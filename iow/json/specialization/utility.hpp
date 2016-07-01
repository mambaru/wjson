#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/serializer/utility.hpp>

namespace iow{ namespace json{

/// /////////////////////////////////////////////////////

template<typename T, int R>
struct raw_value
{
  typedef T target;
  typedef serializerT< raw_value< T, R > > serializer;
};

template<typename T>
struct iterator_pair
{
  typedef T target;
  typedef serializerT< iterator_pair< T > > serializer;
};

template<typename T, typename J>
struct pointer
{
  typedef T target;
  typedef serializerT< pointer< T, J > > serializer;
};

template<typename T, typename V, typename M, M V::* m, typename W >
struct member_value
{
  typedef T target;
  typedef serializerT< member_value<T, V, M, m, W> > serializer;
};

template<typename J, bool SerQ, bool ReqQ>
struct raw_quoted 
{
  typedef typename J::target target;
  typedef serializerRQ< J, SerQ, ReqQ > serializer;
};

template<typename J, bool SerQ , bool ReqQ , int R >
struct quoted
{
  typedef typename J::target target;
  typedef serializerQ< J, value<std::string>, SerQ, ReqQ, R> serializer;
};

}}

