//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace wjson{

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

}
