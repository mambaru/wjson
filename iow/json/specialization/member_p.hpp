//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

namespace iow{ namespace json{

template<typename N, typename T, typename M, typename G, typename W >
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

}}
