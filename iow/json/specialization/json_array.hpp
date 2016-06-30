
// #define CHAR_TO_STRING(CH) #CH

namespace iow{ namespace json{

template<typename C, typename R>
struct array_r;

namespace
{

template<char C>
struct ch2str;

template<>
struct ch2str<'{'>{ static const char* get() { return "{"; } };

template<>
struct ch2str<'}'>{ static const char* get() { return "}"; } };

template<>
struct ch2str<'['>{ static const char* get() { return "["; } };

template<>
struct ch2str<']'>{ static const char* get() { return "]"; } };

}

template<typename K, typename V>
class serializerT< pair<K, V> >
{
public:
  typedef pair<K, V> pair_type;
  typedef typename pair_type::target target;
  typedef typename pair_type::key_serializer key_serializer;
  typedef typename pair_type::value_serializer value_serializer;

  template<typename P>
  P operator()( const target& t, P end)
  {
    end = key_serializer()(t.first, end );
    *(end++)=':';
    end = value_serializer()(t.second, end );
    return end;
  }

  template<typename P>
  P operator()( target& t,  P beg, P end, json_error* e)
  {
    beg = key_serializer()(t.first, beg, end, e );
    beg = parser::parse_space(beg, end, e);
    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);
    if (*beg!=':') 
      return json_error::create<expected_of>(e, end,":", std::distance(beg, end) );
    ++beg;
    beg = parser::parse_space(beg, end, e);
    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);
    beg = value_serializer()(t.second, beg, end, e );
    return beg;
  }
};

template< typename T, char L, char R >
class serializerA
{
  typedef T array_type;
  typedef typename array_type::target_container target_container;
  typedef typename array_type::json_value json_value;
  typedef typename json_value::serializer serializer;
  typedef typename json_value::target target;


public:
  template<typename P>
  P operator()( target_container& t,  P beg, P end, json_error* e)
  {
    
    t.clear();

    if ( parser::is_null(beg, end) )
    {
      // t = target_container();
      return parser::parse_null(beg, end, e);
    }

    array_type().reserve(t);
    
    typename array_type::inserter_iterator bitr = array_type::inserter(t);

    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);
    
    if (*beg!=L) 
      return json_error::create<expected_of>(e, end, ch2str<L>::get()/*std::string(1, L)*/, std::distance(beg, end) );
    ++beg;
    for (;beg!=end;)
    {
      beg = parser::parse_space(beg, end, e);
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      if (*beg==R) break;
      
      target tg;
      beg = serializer()( tg, beg, end, e);
      *(bitr++) = tg;
      
      //beg = serializer()( *(bitr++), beg, end);
      beg = parser::parse_space(beg, end, e);
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      if (*beg==R) break;
      if (*beg!=',')
        return json_error::create<expected_of>(e, end, ",", std::distance(beg, end) );
      ++beg;
    }
    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);
    if (*beg!=R) 
      return json_error::create<expected_of>(e, end, ch2str<R>::get(), std::distance(beg, end) );
    ++beg;
    return beg;
  }

  template<typename P>
  P operator()( const target_container& t, P end)
  {
    *(end++)=L;
    typename target_container::const_iterator itr = t.begin();
    for (;itr!=t.end();)
    {
      end = serializer()(*itr, end);
      ++itr;
      if (itr!=t.end()) *(end++)=',';
    }
    *(end++)=R;
    return end;
  }
};

template< typename T, typename RR >
class serializerT< array_r<T, RR> >
  : public serializerA< array_r<T, RR>, '[', ']'>
{
};

template< typename J, size_t N, typename RR, char L, char R >
class serializerA< array_r< std::array<J,N>, RR>, L, R >
{
  typedef array_r<std::array<J,N>, RR> array_type;
  typedef typename array_type::target_container target_container;
  typedef typename array_type::json_value json_value;
  typedef typename json_value::serializer serializer;
  typedef typename json_value::target target;


public:
  template<typename P>
  P operator()( target_container& t,  P beg, P end, json_error* e)
  {
    if ( parser::is_null(beg, end) )
    {
      for (size_t i = 0; i < N ; ++i)
      {
        //t[i] = target();
        serializer()(t[i], beg, end, e);
      }
      return parser::parse_null(beg, end, e);
    }

    target* bitr = &t[0];
    target* eitr = bitr + N;

    if (beg==end)
      return json_error::create<unexpected_end_fragment>(e, end);
    
    if (*beg!=L) 
      return json_error::create<expected_of>(e, end, ch2str<L>::get(), std::distance(beg, end) );
    
    ++beg;
    for (;beg!=end && bitr!=eitr;)
    {
      beg = parser::parse_space(beg, end);
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      if (*beg==R) break;
      target tg;
      beg = serializer()( tg, beg, end, e);
      *(bitr++) = tg;
      beg = parser::parse_space(beg, end);
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      if (*beg==R) break;
      if (*beg!=',') 
        return json_error::create<expected_of>(e, end, "", std::distance(beg, end) );
      ++beg;
    }
    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);
    if (*beg!=R) 
      return json_error::create<expected_of>(e, end, ch2str<R>::get(), std::distance(beg, end) );
    ++beg;
    return beg;
  }

  template<typename P>
  P operator()( const target_container& t, P end)
  {
    *(end++)=L;
    const target* itr = &t[0];
    const target* iend = itr + N;
    for (;itr!=iend;)
    {
      end = serializer()(*itr, end);
      ++itr;
      if (itr!=iend) *(end++)=',';
    }
    *(end++)=R;
    return end;
  }
};

template< typename J, int N, typename RR, char L, char R >
class serializerA< array_r< J[N], RR>, L, R >
{
  typedef array_r< J[N], RR> array_type;
  typedef typename array_type::target_container target_container;
  typedef typename array_type::json_value json_value;
  typedef typename json_value::serializer serializer;
  typedef typename json_value::target target;


public:
  template<typename P>
  P operator()( target_container& t,  P beg, P end, json_error* e)
  {
    if ( parser::is_null(beg, end) )
    {
      for (int i = 0; i < N ; ++i)
      {
        // #warning БЫЛО: t[i] = target();
        // Подсовываем null каждому значению
        serializer()(t[i], beg, end, e);
      }
      return parser::parse_null(beg, end, e);
    }

    target* bitr = t;
    target* eitr = bitr + N;

    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);
    if (*beg!=L) 
      return json_error::create<expected_of>(e, end, ch2str<L>::get(), std::distance(beg, end) );
      
    ++beg;
    for (;beg!=end && bitr!=eitr;)
    {
      beg = parser::parse_space(beg, end, e);
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      if (*beg==R) break;
      //!!! target tg;
      //!!! beg = serializer()( tg, beg, end, e);
      //!!! *(bitr++) = tg;
      beg = serializer()( *(bitr++), beg, end, e);
      beg = parser::parse_space(beg, end, e);
      if (beg==end) 
        return json_error::create<unexpected_end_fragment>(e, end);
      if (*beg==R) break;
      if (*beg!=',') 
        return json_error::create<expected_of>(e, end, ",", std::distance(beg, end) );
      ++beg;
    }
    if (beg==end) 
      return json_error::create<unexpected_end_fragment>(e, end);
    
    if (*beg!=R) 
      return json_error::create<expected_of>(e, end, ch2str<R>::get(), std::distance(beg, end) );

    ++beg;
    return beg;
  }

  template<typename P>
  P operator()( const target_container& t, P end)
  {
    *(end++)=L;
    const target* itr = t;
    const target* iend = itr + N;
    for (;itr!=iend;)
    {
      end = serializer()(*itr, end);
      ++itr;
      if (itr!=iend) *(end++)=',';
    }
    *(end++)=R;
    return end;
  }
};

}}


