
namespace iow{ namespace json{

/// /////////////////////////////////////////////////////////////////

template<typename T, typename L>
class serializerT< set_enumerator<T, L> >
{
  typedef typename set_enumerator<T, L>::enum_list enum_list;
public:
  template<typename P>
  P operator()( const T& v, P end)
  {
    *(end++)='[';
    end = this->serialize(v, enum_list(), end,true);
    *(end++)=']';
    return end;
  }

  template<typename LL, typename RR, typename P>
  P serialize( const T& v, fas::type_list<LL, RR>, P end, bool isFirst)
  {
    if (LL::value & v)
    {
      if (!isFirst) *(end++)=',';
      *(end++)='"';
      const char* val = LL()();
      for ( ; *val!='\0' ; ++val) *(end++) = *val;
      *(end++)='"';
      return this->serialize(v, RR(), end, false);
    }
    else
    {
      return this->serialize(v, RR(), end, isFirst);
    }
  }

  template<typename P>
  P serialize( const T&, fas::empty_list, P end, bool)
  {
    return end;
  }

  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    for ( ; beg!=end && *beg<=' '; ++beg);
    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);

    if (*beg =='"') 
    {
      ++beg;
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      P first = beg;
      for ( ; beg!=end && *beg!='"'; ++beg);
      if (beg==end)
        return json_error::create<unexpected_end_fragment>(e, end);

      if (*beg!='"') 
        return json_error::create<expected_of>(e, end, "\"", std::distance(beg, end) );

      this->deserialize(v, enum_list(), first, beg);
      ++beg;
    } else if (*beg == '[') {
      ++beg;
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      beg = this->deserialize_arr(v, enum_list(), beg, end, e);
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      if (*beg!=']') 
        return json_error::create<expected_of>(e, end, "]", std::distance(beg, end) );

      ++beg;
    } 
    else
      return json_error::create<expected_of>(e, end, "\"", std::distance(beg, end) );

    return beg;
  }

  template<typename LL, typename RR, typename P>
  P deserialize_one( T& v, fas::type_list<LL, RR>, P beg, P end)
  {
    P first = beg;
    const char *pstr = LL()();
    for ( ; beg!=end && *pstr!='\0' && *pstr==*beg; ++beg, ++pstr);
    if ( beg==end && *pstr=='\0') {
      v = (T)(v|LL::value);
      return end;
    } else if (*beg==',' && *pstr=='\0') {
      beg++;
      v = (T)(v|LL::value);
      return beg;
    } else {
      return deserialize_one(v, RR(), first, end);
    }
  }

  template<typename P>
  P deserialize_one( T& , fas::empty_list, P beg, P /*end*/)
  {
    return beg;
  }

public:
  /**
    * для десериализации строки вида "One,Two,Three"
    */
  template<typename LL, typename RR, typename P>
  void deserialize( T& v, fas::type_list<LL, RR> tl, P beg, P end) {
    v = T();
    P beg_prev = end;
    while (beg != beg_prev && beg!=end) {
      beg_prev = beg;
      beg = deserialize_one(v, tl, beg, end);
    }
  }
  /**
  * для десериализации массива вида ["One","Two","Three"]
  */
  template<typename LL, typename RR, typename P>
  P deserialize_arr( T& v, fas::type_list<LL, RR> /*tl*/, P beg, P end, json_error* e) {
    v = T();
    char lastChar = ' ';
    while (*beg=='"') {
      ++beg;
      if (beg==end)
        return json_error::create<unexpected_end_fragment>(e, end);
      P first = beg;
      for ( ; beg!=end && *beg!='"'; ++beg);
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      if (*beg!='"') 
        return json_error::create<expected_of>(e, end, "\"", std::distance(beg, end) );
      this->deserialize_one(v, enum_list(), first, beg);
      beg++;
      lastChar = *beg;
      if (lastChar==',') beg++;
    }
    if (lastChar==',') 
      return json_error::create<unexpected_end_fragment>(e, end);
    return beg;
  }

};

}}

