//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace wjson{

template<typename N, typename T, typename M, M T::* m, typename J >
struct member: N
{
  typedef T object;
  typedef M type;
  typedef M target;
  typedef J target_json;
  typedef typename J::serializer serializer;
  typedef typename J::target member_target;
  const char* operator()() const { return N::operator()(); }

  member_target* ptr(T* t) { return static_cast<member_target*>(t->*m);};
  const member_target* ptr(const T* t) const { return static_cast< const member_target*>(t->*m);};
  member_target& ref(T& t) { return static_cast<member_target&>(t.*m); };
  const member_target& ref(const T& t) const { return static_cast< const member_target&>(t.*m);};
};

}
