//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/parser.hpp>
#include <wjson/error.hpp>

namespace wjson{

template<typename T, typename L, typename Mode>
class serializerT< object<T, L, Mode> >
{
  typedef object<T, L, Mode> object_type;

public:

  template<typename P>
  P operator()( const T& t, P end)
  {
    *(end++)='{';
    end = this->serialize_members(t, end, L() );
    *(end++)='}';
    return end;
  }

  template<typename P>
  P operator()( T& t, P beg, P end, json_error* e)
  {
    if ( parser::is_null(beg, end) )
    {
      t = T();
      return parser::parse_null(beg, end, e);
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *beg != '{' ) 
    {
      if (*beg=='[')
      {
        beg = parser::parse_space(++beg, end, e);
        if (*beg!=']')
          return create_error<error_code::ExpectedOf>(e, end, "]", std::distance(beg, end) );
        // fix for php. {} <=> []
        return ++beg;
      }
      return create_error<error_code::ExpectedOf>(e, end, "{", std::distance(beg, end)  );
    }
    ++beg;

    beg = parser::parse_space(beg, end, e);
    if ( beg==end ) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    
    if ( *beg != '}')
    {
       beg = this->unserialize_members(t, beg, end, L(), false, e );
       if ( beg==end ) 
         return create_error<error_code::UnexpectedEndFragment>(e, end);
       beg = parser::parse_space(beg, end, e);
       if ( beg==end ) 
         return create_error<error_code::UnexpectedEndFragment>(e, end);
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *(beg++) != '}' ) 
      return create_error<error_code::ExpectedOf>(e, end,"}", std::distance(beg, end) + 1 );

    return beg;
  }

private:

  template<typename P, typename C, typename R>
  P serialize_members( const T& t, P end, fas::type_list<C, R> )
  {
    end = this->serialize_member(t, end, C());
    *(end++)=',';
    return this->serialize_members(t, end, R() );
  }

  template<typename P, typename C>
  P serialize_members( const T& t, P end, fas::type_list<C, fas::empty_list> )
  {
    return this->serialize_member(t, end, C());
  }

  template<typename P>
  P serialize_members( const T&, P end, fas::empty_list )
  {
    return end;
  }

  template<typename P, typename M >
  P serialize_member_name( const T& , P end, M memb )
  {
    const char* name = memb();
    *(end++)='"';
    for (;*name!='\0'; ++name) *(end++) = *name;
    *(end++)='"';
    *(end++) = ':';
    return end;
  }

  template<typename P, typename N, typename G, typename M, M G::* m, typename W >
  P serialize_member( const T& t, P end, const member<N, G, M, m, W>& memb )
  {
    end = this->serialize_member_name(t, end, memb);

    typedef typename member<N, G, M, m, W>::serializer serializer;
    return serializer()( memb.ref(t), end );
  }

  template<typename P, typename N, typename G, typename M, typename GT, typename W >
  P serialize_member( const T& t, P end, const member_p<N, G, M, GT, W>& memb )
  {
    end = this->serialize_member_name(t, end, memb);
    typedef typename member_p<N, G, M, GT, W>::serializer serializer;
    return serializer()( memb.get(t), end );
  }


  template<typename P, typename ML, typename MR, bool RU >
  P serialize_member( const T& t, P end, const member_if<ML, MR, RU>&  )
  {
    typedef typename ML::type typeL;
    if ( !( this->get_value_(t, ML()) == typeL() ) )
      return this->serialize_member( t, end, ML() );
    return  this->serialize_member( t, end, MR() );
  }

private:

  template<typename N, typename G, typename M, M G::* m, typename W >
  M get_value_( const T& t, member<N, G, M, m, W> memb )
  {
    return memb.ref(t);
  }

  template<typename N, typename G, typename M, typename GT, typename W >
  M get_value_( const T& t, member_p<N, G, M, GT, W> memb )
  {
    return memb.get(t);
  }

  template<typename P, typename C, typename R>
  P unserialize_members( T& t, P beg, P end, fas::type_list<C, R>, bool search /*= false*/, json_error* e )
  {
    bool unserialized = false;
    beg = this->unserialize_member( t, beg, end, C(), unserialized, e );

    if ( !unserialized )
    {
      if ( !search ) // Организуем поиск с начала списка
        beg = this->unserialize_members( t, beg, end, L(), true, e );
      else // Продолжаем поиск
        return this->unserialize_members( t, beg, end, R(), true, e );
    }
    else if (search)
      return beg;

    beg = parser::parse_space(beg, end, e);

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *beg == ',' )
    {
      ++beg;
      beg = parser::parse_space(beg, end, e);

      if ( unserialized )
        beg = this->unserialize_members( t, beg, end, R(), false, e );
      else
        beg = this->unserialize_members( t, beg, end, fas::type_list<C, R>() , false, e );
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if ( *beg != '}' )
      return create_error<error_code::ExpectedOf>(e, end,"}", std::distance(beg, end) );

    return beg;
  }

  template<typename P>
  P unserialize_members( T& , P beg, P end, fas::empty_list, bool search /*= false*/, json_error* e )
  {
    if ( !search )
    {
      beg = parser::parse_space(beg, end, e);
      if ( beg==end ) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);

      if ( *beg=='}' ) return beg;
      for(;;)
      {
        P member_beg = beg;
        beg = parser::parse_member(beg, end, e);
        beg = parser::parse_space(beg, end, e);
        if ( beg==end ) 
          return create_error<error_code::UnexpectedEndFragment>(e, end);
        if ( *beg=='}' ) 
        {
          if ( fas::same_type<Mode, nonstrict_mode>::value  )
            return beg;
          // Ошибка, неизвестный метод, для srict_mode
          return create_error<error_code::InvalidMember>(e, end, std::distance(member_beg, end));
        }
        if ( *beg!=',' ) 
          return create_error<error_code::ExpectedOf>(e, end, ",", std::distance(beg, end) );

        ++beg;
        beg = parser::parse_space(beg, end, e);
      }
    }
    else if ( fas::same_type<Mode, nonstrict_mode>::value  )
    {
      // если организован поиск и не нашли то пропускаем

      beg = parser::parse_member(beg, end, e);
      beg = parser::parse_space(beg, end, e);
      return beg;
    }
    else
    {
      if ( beg==end ) 
        return create_error<error_code::UnexpectedEndFragment>(e, end);
      if ( *beg=='}' ) 
        return beg;
      // Ошибка, неизвестный метод, для srict_mode
      return create_error<error_code::InvalidMember>(e, end, std::distance(beg, end));
    }
  }

  template<typename P, typename M >
  P unserialize_member_name( T& , P beg, P end, M memb, bool& unserialized, json_error* e )
  {
    const char* name = memb();
    P start = beg;
    if ( !parser::is_string(beg, end) )
      return create_error<error_code::ExpectedOf>(e, end, "\"", std::distance(beg, end) );
      
    ++beg;
    unserialized = true;
    for ( ; beg!=end && *name!='\0' && *beg==*name && *beg!='"'; ++name, ++beg)
    {
      if (*name!=*beg) 
      {
        unserialized = false;
        break;
      }
    }

    if (beg==end) 
      return create_error<error_code::UnexpectedEndFragment>(e, end);

    if (*beg!='"' || *name!='\0') 
      unserialized = false;

    if ( !unserialized ) 
      return start;

    ++beg;
    beg = parser::parse_space(beg, end, e);
    if (beg==end) 
       return create_error<error_code::UnexpectedEndFragment>(e, end);

    if (*beg!=':') 
      return create_error<error_code::ExpectedOf>(e, end, ":", std::distance(beg, end) );

    ++beg;
    beg = parser::parse_space(beg, end, e);

    if (beg==end) 
       return create_error<error_code::UnexpectedEndFragment>(e, end);

    return beg;
  }


  template<typename P, typename N, typename G, typename M, M G::* m, typename W >
  P unserialize_member( T& t, P beg, P end, member<N, G, M, m, W> memb, bool& unserialized, json_error* e )
  {
    beg = this->unserialize_member_name(t, beg, end, memb, unserialized, e);
    if ( !unserialized )
      return beg;
    typedef typename member<N, G, M, m, W>::serializer serializer;
    return serializer()( memb.ref(t), beg, end, e);
  }

  template<typename P, typename N, typename G, typename M, typename GT, typename W >
  P unserialize_member( T& t, P beg, P end, member_p<N, G, M, GT, W> memb, bool& unserialized, json_error* e )
  {
    beg = this->unserialize_member_name(t, beg, end, memb, unserialized, e);
    if ( !unserialized )
      return beg;
    typedef typename member_p<N, G, M, GT, W>::serializer serializer;
    M value = M();
    beg = serializer()( value, beg, end, e);
    memb.set(t, value);
    return beg;
  }

  template<typename P, typename ML, typename MR >
  P unserialize_member( T& t, P beg, P end, member_if<ML, MR, true>, bool& unserialized, json_error* e )
  {
    static const char *snull = "null";
    json_error ee;
    P itr = this->unserialize_member(t, beg, end, MR(), unserialized, &ee);
    if ( !ee )
    {
      typename ML::serializer()(ML().ref(t), snull, snull+4, 0);
      return itr;
    }
    typename MR::serializer()(MR().ref(t), snull, snull+4, 0);
    return this->unserialize_member(t, beg, end, ML(), unserialized, e);
  }

  template<typename P, typename ML, typename MR >
  P unserialize_member( T& t, P beg, P end, member_if<ML, MR, false>, bool& unserialized, json_error* e )
  {
    static const char *snull = "null";
    json_error ee;
    P itr = this->unserialize_member(t, beg, end, ML(), unserialized, &ee);
    if ( !ee )
    {
      typename MR::serializer()(MR().ref(t), snull, snull+4, 0);
      return itr;
    }

    typename ML::serializer()(ML().ref(t), snull, snull+4, 0);
    return this->unserialize_member(t, beg, end, MR(), unserialized, e);
  }
};

}
