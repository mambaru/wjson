#include <iostream>
#include <iow/io/aux/write_buffer.hpp>
#include <fas/testing.hpp>
#include <algorithm>
#include <cstring>

struct _data_line_;
struct _generator_;
struct _init_line_;

struct _data_size_;
struct _data_line_init_;
struct _test_options_;

struct data_line_test_options: iow::io::write_buffer_options< std::string /*std::vector<char>*/ >
{
  size_t data_size;
  size_t data_count;
};

data_line_test_options getopt(
  size_t bufsize,
  size_t maxbuf, 
  size_t minbuf, 
  //size_t wrnsize,
  //size_t maxsize,
  bool  first_as_is,
  size_t data_size,
  size_t data_count
)
{
  data_line_test_options dlto;
  dlto.bufsize = bufsize;
  dlto.maxbuf = maxbuf; 
  dlto.minbuf = minbuf;
  // dlto.wrnsize = wrnsize;
  //dlto.maxsize = maxsize;
  dlto.first_as_is = first_as_is;
  dlto.data_size = data_size;
  dlto.data_count = data_count;
  return dlto;
}

struct generator
{
  template<typename T>
  typename T::aspect::template advice_cast<_data_line_>::type::data_ptr
  operator()(T&, size_t offset, size_t size) 
  {
    typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
    typedef typename data_line_t::data_type data_type;
    typedef typename data_line_t::data_ptr data_ptr;
    
    data_ptr d = std::make_unique<data_type>();
    for ( size_t j = offset; j < offset + size; j++)
      d->push_back('A'+j%10);
    
    return std::move(d);
  }
};

template<typename T>
typename T::aspect::template advice_cast<_data_line_>::type::data_ptr
generate(T& t, size_t offset, size_t size)
{
  return t.get_aspect().template get<_generator_>()(t, offset, size);
}

struct init_line
{
  template<typename T>
  void operator()(T& t)
  {
    using namespace fas::testing;
    auto& data_line = t.get_aspect().template get<_data_line_>();
    typedef typename std::remove_reference<decltype(data_line)>::type data_line_t;
    typedef typename data_line_t::data_ptr data_ptr;

    auto opt = t.get_aspect().template get<_test_options_>();
    data_line.clear();
    data_line.set_options( *opt );
    data_ptr first;
    for (size_t i = 0; i < opt->data_count; i++ )
    {
      //t << message("generate: ") << i;
      data_ptr d = t.get_aspect().template get<_generator_>()( t, i, opt->data_size );
      data_line.attach( std::move(d) );
    }
  }
};

UNIT(init_test, "")
{
  using namespace fas::testing;
  t.get_aspect().template get<_init_line_>()(t);

  auto& data_line = t.get_aspect().template get<_data_line_>();
  typedef typename std::remove_reference<decltype(data_line)>::type data_line_t;
  auto test_opt = t.get_aspect().template get<_test_options_>();
  data_line.set_options(*test_opt);
  typename data_line_t::options_type dl_opt;
  data_line.get_options(dl_opt);
  
  /*
  const char* buf1 = reinterpret_cast<const char*>( &*test_opt);
  const char* buf2 = reinterpret_cast<const char*>( &dl_opt );
  
  int cmpres = std::memcmp( buf1,  buf2, sizeof(dl_opt) );
  bool opt_test = ( 0 ==  cmpres) ;
  t << is_true<assert>( opt_test ) << FAS_TESTING_FILE_LINE;
  t << stop;
  */

  size_t linesize = data_line.size();
  size_t testsize = test_opt->data_size * test_opt->data_count;
  t << is_true<assert>( linesize == testsize ) << " " << linesize << "!=" << testsize << ":" << FAS_TESTING_FILE_LINE;
  
  linesize = 0;
  size_t linecount = 0;
  size_t initcount = data_line.count();
  auto d = data_line.next();
  
  while ( d.first != nullptr )
  {
    linesize += d.second;
    data_line.confirm(d);
    if ( data_line.offset()==0 )
      ++linecount;

    t << equal<assert>( data_line.size()  + linesize,  testsize  ) << data_line.size()  + linesize << "!=" << testsize << FAS_TESTING_FILE_LINE;
    t << equal<assert>( data_line.count() + linecount, initcount ) << data_line.count() + linecount<< "!=" << initcount << FAS_TESTING_FILE_LINE;
    t << stop;
    d = data_line.next();
  }
  t << equal<assert, size_t>( data_line.size(),  0 ) << data_line.size() << FAS_TESTING_FILE_LINE;
  t << equal<assert, size_t>( data_line.count(), 0 ) << data_line.count() << FAS_TESTING_FILE_LINE;
}

UNIT(nobuf_test, "non-buffering mode")
{
  using namespace fas::testing;
  auto& data_line = t.get_aspect().template get<_data_line_>();
  typedef typename std::remove_reference<decltype(data_line)>::type data_line_t;
  auto test_opt = t.get_aspect().template get<_test_options_>();
    
  data_line.clear();
  data_line.set_options( *test_opt);
  typename data_line_t::options_type dl_opt;
  data_line.get_options(dl_opt);
  //auto dl_opt = data_line.get_options();

  size_t maxbuf = dl_opt.maxbuf;
  size_t cursize = 1;
  for (size_t c = 0; c < test_opt->data_count; ++c)
  {
    auto d1 = generate(t, c, cursize);
    auto d2 = generate(t, c, cursize);
    data_line.attach( std::move(d1) );
    auto d3 = data_line.next();
    t << is_true<assert>( d3.first!=nullptr ) << "c: " << c << ": " << FAS_TESTING_FILE_LINE;
    t << stop;
    t << equal<assert, std::string>( d3.first, *d2 ) << "c: " << c << ", " << d3.first << "!=" << *d2 << FAS_TESTING_FILE_LINE;
    data_line.confirm(d3);
    t << is_true<assert>( data_line.size()==0 )  << FAS_TESTING_FILE_LINE;
    t << is_true<assert>( data_line.count()==0 ) << FAS_TESTING_FILE_LINE;
    t << stop;
    cursize += 1;
    if ( cursize > maxbuf )
      cursize = 1;
  }
}

UNIT(fullbuf_test, "full-buffering mode")
{
  // Отключены опции:
  // except_first = false
  // except_confirm = false

  using namespace fas::testing;
  typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
  typedef typename data_line_t::data_type data_type;

  data_line_t& data_line = t.get_aspect().template get<_data_line_>();
  //typedef typename decltype(data_line)::data_ptr data_ptr;

  auto test_opt = t.get_aspect().template get<_test_options_>();
  //!!! test_opt.except_first = false;
  //!!! test_opt.except_confirm = false;

  data_line.clear();
  data_line.set_options( *test_opt);
  typename data_line_t::options_type dl_opt;
  data_line.get_options(dl_opt);
  //auto dl_opt = data_line.get_options();

  size_t maxbuf = dl_opt.maxbuf*2;
  size_t cursize = 1;
  for (size_t c = 0; c < test_opt->data_count; ++c)
  {
    auto d1 = generate(t, 0, cursize); // исходные данные и результат
    auto d2 = generate(t, 0, cursize); // для проверки
    data_line.attach( std::move(d1) );  // следующая порция
    auto d3 = data_line.next();        // следующая порция
    while ( d3.first != nullptr )
    {
      if ( d1==nullptr )
      {
        d1 = std::make_unique<data_type>( d3.first, d3.first + d3.second );
      }
      else
      {
        std::copy( d3.first, d3.first + d3.second, std::back_inserter(*d1) );
      }
      data_line.confirm(d3);
      d3 = data_line.next();
    }
    
    if ( d1!=nullptr )
    {
      t << equal<assert, std::string>( *d1, *d2 ) << "c: " << c << ", " << *d1 << "!=" << *d2 << FAS_TESTING_FILE_LINE;
      t << stop;
    }
    else
    {
      t << fatal("d1==nullptr");
    }
    cursize += 1;
    if ( cursize > maxbuf )
      cursize = 1;
  }
}

UNIT(ignore_first_test, "buffering with ignore first flag mode")
{
  using namespace fas::testing;
  typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
  auto& data_line = t.get_aspect().template get<_data_line_>();
  auto test_opt = t.get_aspect().template get<_test_options_>();
  test_opt->first_as_is = true;
  //!! test_opt.except_confirm = true;
  data_line.clear();
  data_line.set_options(*test_opt);
  typename data_line_t::options_type dl_opt;
  data_line.get_options(dl_opt);
  
  size_t cursize = 1;
  size_t maxbuf = (dl_opt.maxbuf * 3) / 2;
  for (size_t c = 0; c < test_opt->data_count; ++c)
  {
    auto d1 = generate(t, c, cursize);
    auto d2 = generate(t, c, cursize);
    data_line.attach( std::move(d1) );
    auto d3 = data_line.next();
    t << is_true<assert>( d3.first!=nullptr ) << "c: " << c << ": " << FAS_TESTING_FILE_LINE;
    t << stop;
    t << equal<assert, std::string>( d3.first, *d2 ) << "c: " << c << ", " << d3.first << "!=" << *d2 << FAS_TESTING_FILE_LINE;
    data_line.confirm(d3);
    t << is_true<assert>( data_line.size()==0 )  << FAS_TESTING_FILE_LINE;
    t << is_true<assert>( data_line.count()==0 ) << FAS_TESTING_FILE_LINE;
    t << stop;
    cursize += 1;
    if ( cursize > maxbuf )
      cursize = 1;
    
  }
}

UNIT(partconfirm_test, "partconfirm_test")
{
  using namespace fas::testing;
  typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
  auto& data_line = t.get_aspect().template get<_data_line_>();
  auto test_opt = t.get_aspect().template get<_test_options_>();
  typedef typename std::remove_reference< decltype( data_line ) >::type::data_type data_type;
  
  //!! test_opt.except_first   = false;
  //!! test_opt.except_confirm = false;
  data_line.clear();
  data_line.set_options(*test_opt);
  typename data_line_t::options_type dl_opt;
  data_line.get_options(dl_opt);

  
  size_t cursize = 1;
  size_t maxbuf = (dl_opt.maxbuf * 3) / 2;
  data_type result1, result2;
  for (size_t c = 0; c < test_opt->data_count; ++c)
  {
    auto d1 = generate(t, c, cursize);
    std::copy( d1->begin(), d1->end(), std::back_inserter(result1));
    data_line.attach( std::move(d1) );
    cursize += 1;
    if ( cursize > maxbuf )
      cursize = 1;
  }
  
  auto d2 = data_line.next();
  while( d2.first!=nullptr )
  {
    size_t size = d2.second/3;
    if ( size==0 && d2.second!=0 )
      size = 1;
    std::copy( d2.first, d2.first + size, std::back_inserter(result2) );
    d2.second = size;
    data_line.confirm(d2);
    d2 = data_line.next();
  }
  
  t <<   equal<assert>( result1, result2 )     << result1 << "!=" << result2 << FAS_TESTING_FILE_LINE;
  t << is_true<assert>( data_line.size()==0 )  << data_line.size() << FAS_TESTING_FILE_LINE;
  t << is_true<assert>( data_line.count()==0 ) << FAS_TESTING_FILE_LINE;
}

BEGIN_SUITE(aux,"aux suite")
  ADD_UNIT(init_test)
  ADD_UNIT(nobuf_test)
  ADD_UNIT(fullbuf_test)
  ADD_UNIT(ignore_first_test)
  ADD_UNIT(partconfirm_test)
  ADD_VALUE(_test_options_, std::shared_ptr<data_line_test_options> )
  ADD_VALUE(_data_line_, ::iow::io::write_buffer< std::string > )
  ADD_ADVICE(_generator_, generator)
  ADD_ADVICE(_init_line_, init_line)
END_SUITE(aux)

::fas::testing::suite_counts fas_my_suite_run(int , char*[])
{
  data_line_test_options optlist[]={
  //       bufsize maxbuf minbuf first_as_is data_size data_count
    getopt( 0,      0,     0,    false,      10,       10   ),
    getopt( 10,     0,     0,    false,      100,      100  ),
    getopt( 10,     10,    10,   false,      11,       1    ),
    getopt( 10,     20,    5,    false,      11,       33   ),
    getopt( 10,     20,    5,    false,      100,      100  ),
    getopt( 33,     34,    32,   false,      1000,     1000 ),
  };
  std::cout << sizeof(optlist) <<std::endl;
  ::fas::testing::suite_counts sc;
  fas_aux_suite as;
  
  as.get_aspect().get<_test_options_>() 
    = std::make_shared<data_line_test_options>();
    
  auto opt = as.get_aspect().get<_test_options_>();
  for (size_t i =0 ; i < std::extent< decltype(optlist) >::value; i++)
  {
    std::cout << std::endl << i << std::endl;
    *opt=optlist[i];
    //opt->fix();
    as.run();
    sc += as.counts();
  }
  return sc;
}


BEGIN_TEST
  //RUN_SUITE(aux)
  RUN_SUITE(my)
END_TEST
