
namespace iow{ namespace json{

/// ////////////////////////////////////////////////////////////

template<typename T, typename V, typename M, M V::* m, typename W >
struct serializerT< member_value<T, V, M, m, W> >
{
  template<typename P>
  P operator()( const T& t, P end)
  {
    return typename W::serializer()( static_cast<const V&>(t).*m, end);
  }

  template<typename P>
  P operator()( T& t, P beg, P end)
  {
    return typename W::serializer()( static_cast<V&>(t).*m, beg, end);
  }
};


/// ////////////////////////////////////////////////////////////

template<typename T, typename L>
class serializerT< object<T, L> >
{
  typedef object<T, L> object_type;

public:

  serializerT(){}

  /** serialization:
   * end - back inserter iterator
   */
  template<typename P>
  P operator()( const T& t, P end)
  {
    *(end++)='{';
    end = serialize_members(t, end, L() );
    *(end++)='}';
    return end;
  }

  /** deserialization:
    * beg, end - forward iterator
    */
  template<typename P>
  P operator()( T& t, P beg, P end, json_error* e)
  {
    if ( parser::is_null(beg, end) )
    {
      t = T();
      return parser::parse_null(beg, end, e);
    }

    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);

    if ( *(beg++) != '{' ) 
      return json_error::create<expected_of>(e, end, "{", std::distance(beg, end) + 1 );

    beg = parser::parse_space(beg, end, e);
    if ( beg==end ) 
      return json_error::create<unexpected_end_fragment>(e, end);

    
    if ( *beg != '}')
    {
       beg = unserialize_members(t, beg, end, L(), false, e );
       if ( beg==end ) 
         return json_error::create<unexpected_end_fragment>(e, end);
       beg = parser::parse_space(beg, end, e);
       if ( beg==end ) 
         return json_error::create<unexpected_end_fragment>(e, end);
    }

    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);

    if ( *(beg++) != '}' ) 
      return json_error::create<expected_of>(e, end,"}", std::distance(beg, end) + 1 );

    return beg;
  }

private:

  template<typename P, typename C, typename R>
  P serialize_members( const T& t, P end, fas::type_list<C, R> )
  {
    end = serialize_member(t, end, C());
    *(end++)=',';
    return serialize_members(t, end, R() );
  }

  template<typename P, typename C>
  P serialize_members( const T& t, P end, fas::type_list<C, fas::empty_list> )
  {
    return serialize_member(t, end, C());
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
    end = serialize_member_name(t, end, memb);

    typedef typename member<N, G, M, m, W>::serializer serializer;
    return serializer()( memb.ref(t), end );
  }

  template<typename P, typename N, typename G, typename M, typename GT, typename W >
  P serialize_member( const T& t, P end, const member_p<N, G, M, GT, W>& memb )
  {
    end = serialize_member_name(t, end, memb);
    typedef typename member_p<N, G, M, GT, W>::serializer serializer;
    return serializer()( memb.get(t), end );
  }


  template<typename P, typename ML, typename MR, bool RU >
  P serialize_member( const T& t, P end, const member_if<ML, MR, RU>& memb )
  {
    typedef typename ML::type typeL;
    if ( !( _get_value(t, ML()) == typeL() ) )
      return serialize_member( t, end, ML() );
    return serialize_member( t, end, MR() );
  }

private:

  template<typename N, typename G, typename M, M G::* m, typename W >
  M _get_value( const T& t, member<N, G, M, m, W> memb )
  {
    return memb.ref(t);
  }

  template<typename N, typename G, typename M, typename GT, typename W >
  M _get_value( const T& t, member_p<N, G, M, GT, W> memb )
  {
    return memb.get(t);
  }

  template<typename P, typename C, typename R>
  P unserialize_members( T& t, P beg, P end, fas::type_list<C, R>, bool search /*= false*/, json_error* e )
  {
    bool unserialized = false;
    beg = unserialize_member( t, beg, end, C(), unserialized, e );

    if (!unserialized)
    {
      if ( !search ) // Организуем поиск с начала списка
        beg = unserialize_members( t, beg, end, L(), true, e );
      else // Продолжаем поиск
        return unserialize_members( t, beg, end, R(), true, e );
    }
    else if (search)
      return beg;

    beg = parser::parse_space(beg, end, e);

    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);

    if ( *beg == ',' )
    {
      ++beg;
      beg = parser::parse_space(beg, end, e);

      if ( unserialized )
        beg = unserialize_members( t, beg, end, R(), false, e );
      else
        beg = unserialize_members( t, beg, end, fas::type_list<C, R>() , false, e );
    }

    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);

    if ( *beg != '}' )
      return json_error::create<expected_of>(e, end,"}", std::distance(beg, end) );

    return beg;
  }


  template<typename P>
  P unserialize_members( T& , P beg, P end, fas::empty_list, bool search /*= false*/, json_error* e )
  {
    if ( !search )
    {
      beg = parser::parse_space(beg, end, e);
      if ( beg==end ) 
        return json_error::create<unexpected_end_fragment>(e, end);

      if ( *beg=='}' ) return beg;
      for(;;)
      {
        beg = parser::parse_member(beg, end, e);
        beg = parser::parse_space(beg, end, e);
        if ( beg==end ) 
          return json_error::create<unexpected_end_fragment>(e, end);
        if ( *beg=='}' ) return beg;
        if ( *beg!=',' ) 
          return json_error::create<expected_of>(e, end, ",", std::distance(beg, end) );

        ++beg;
        beg = parser::parse_space(beg, end, e);
      }
    }
    else
    {
      // если организован поиск и не нашли то пропускаем

      beg = parser::parse_member(beg, end, e);
      beg = parser::parse_space(beg, end, e);
      return beg;
    }
  }

  template<typename P, typename M >
  P unserialize_member_name( T& , P beg, P end, M memb, bool& unserialized, json_error* e )
  {
    const char* name = memb();
    P start = beg;
    if ( !parser::is_string(beg, end) )
      return json_error::create<expected_of>(e, end, "\"", std::distance(beg, end) );
      
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
      return json_error::create<unexpected_end_fragment>(e, end);

    if (*beg!='"' || *name!='\0') 
      unserialized = false;

    if ( !unserialized ) return start;
    ++beg;
    beg = parser::parse_space(beg, end, e);
    if (beg==end) 
       return json_error::create<unexpected_end_fragment>(e, end);

    if (*beg!=':') 
      return json_error::create<expected_of>(e, end, ":", std::distance(beg, end) );
      
    ++beg;
    beg = parser::parse_space(beg, end, e);

    if (beg==end) 
       return json_error::create<unexpected_end_fragment>(e, end);

    return beg;
  }


  template<typename P, typename N, typename G, typename M, M G::* m, typename W >
  P unserialize_member( T& t, P beg, P end, member<N, G, M, m, W> memb, bool& unserialized, json_error* e )
  {
    beg = unserialize_member_name(t, beg, end, memb, unserialized, e);
    if ( !unserialized )
      return beg;
    typedef typename member<N, G, M, m, W>::serializer serializer;
    return serializer()( memb.ref(t), beg, end, e);
  }

  template<typename P, typename N, typename G, typename M, typename GT, typename W >
  P unserialize_member( T& t, P beg, P end, member_p<N, G, M, GT, W> memb, bool& unserialized, json_error* e )
  {
    beg = unserialize_member_name(t, beg, end, memb, unserialized, e);
    if ( !unserialized )
      return beg;
    typedef typename member_p<N, G, M, GT, W>::serializer serializer;
    M value = M();
    beg = serializer()( value, beg, end);
    memb.set(t, value);
    return beg;
  }

  template<typename P, typename ML, typename MR >
  P unserialize_member( T& t, P beg, P end, member_if<ML, MR, true>, bool& unserialized, json_error* e )
  {
    return unserialize_member(t, beg, end, MR(), unserialized, e);
  }

  template<typename P, typename ML, typename MR >
  P unserialize_member( T& t, P beg, P end, member_if<ML, MR, false>, bool& unserialized, json_error* e )
  {
    return unserialize_member(t, beg, end, ML(), unserialized, e);
  }

};

}}
