#pragma once

#include <iow/json/predef.hpp>
#include <iow/json/serializer/object.hpp>

namespace iow{ namespace json{

template<typename J>
struct base: J::member_list {};

template<typename T, typename L>
struct object
{
  typedef T target;
  typedef serializerT< object<T, L> > serializer;
  typedef typename fas::normalize<L>::type member_list;
};

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

template<typename T, typename L>
struct object_array
{
  typedef T target;
  typedef serializerT< object_array<T, L> > serializer;
  typedef typename fas::normalize<L>::type member_list;
};

template<typename T, typename M, M T::* m, typename W >
struct member_array
{
  typedef T target;
  typedef M type;
  typedef W wrapper;
  typedef typename W::serializer serializer;
  typedef typename W::target wrapper_target;
  
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
