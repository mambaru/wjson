//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <fas/system/nullptr.hpp>
#include <memory>

namespace wjson{

template<typename T, typename J>
class serializerT< pointer<T, J> >
{
public:
  template< typename P>
  P operator()( const T& ptr, P end) const 
  {
    // Можно обычный указатель
    if ( ptr != fas_nullptr )
      return typename J::serializer()( *ptr, end);
    
    *(++end)='n';
    *(++end)='u';
    *(++end)='l';
    *(++end)='l';
    return end;
  }

#if __cplusplus >= 201103L

  template< typename Type, typename P>
  P operator() ( std::unique_ptr<Type>& ptr, P beg, P end, json_error* e ) const
  {
    // Только умный
    if (beg!=end && *beg!='n')
    {
      ptr = std::unique_ptr<Type>(new Type());
      return typename J::serializer()( *ptr, beg, end, e);
    }
    else
    {
      ptr=fas_nullptr;
      for (int i=0; (i < 4) && ( beg!=end ); ++i, ++beg);
    }
    return beg;
  }
  
  template< typename Type, typename P>
  P operator() ( std::shared_ptr<Type>& ptr, P beg, P end, json_error* e ) const
  {
    // Только умный
    if (beg!=end && *beg!='n')
    {
      ptr = std::make_shared<Type>();
      return typename J::serializer()( *ptr, beg, end, e);
    }
    else
    {
      ptr = fas_nullptr;
      for (int i=0; (i < 4) && (beg!=end); ++i, ++beg);
    }
    return beg;
  }
#endif // __cplusplus >= 201103L

};

}
