#include <iow/aux/data_line.hpp>
#include <fas/testing.hpp>
#include <algorithm>
#include <cstring>

struct _data_line_;
struct _generator_;
struct _init_line_;

struct _data_size_;
struct _data_line_init_;
struct _test_options_;

struct data_line_test_options: iow::data_line_options
{
  size_t data_size;
  size_t data_count;
};

data_line_test_options getopt(
  size_t bufsize,
  size_t maxbuf, 
  size_t minbuf, 
  size_t wrnsize,
  size_t maxsize,
  bool except_first,
  bool except_confirm,
  size_t data_size,
  size_t data_count
)
{
  data_line_test_options dlto;
  dlto.bufsize = bufsize;
  dlto.maxbuf = maxbuf; 
  dlto.minbuf = minbuf;
  dlto.wrnsize = wrnsize;
  dlto.maxsize = maxsize;
  dlto.except_first = except_first;
  dlto.except_confirm = except_confirm;
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
    for ( size_t j=offset; j < offset + size; j++)
      d->push_back('A'+j%10);
    //d->push_back('A');
    return std::move(d);
  }
};

template<typename T>
typename T::aspect::template advice_cast<_data_line_>::type::data_ptr
generate(T& t, size_t offset, size_t size)
{
  return t.get_aspect().template get<_generator_>()(t, offset, size);
}

//UNIT(data_line_init, "")
struct init_line
{
  template<typename T>
  void operator()(T& t)
  {
    auto& data_line = t.get_aspect().template get<_data_line_>();
    typedef typename std::remove_reference<decltype(data_line)>::type data_line_t;
    //typedef typename data_line_t::data_type data_type;
    typedef typename data_line_t::data_ptr data_ptr;

    auto& opt = t.get_aspect().template get<_test_options_>();

    data_line.clear();
    data_line.set_options(opt);
    static_cast<iow::data_line_options&>(opt) = data_line.options();

    data_ptr first;
    for (size_t i =0; i < opt.data_count; i++)
    {
      data_ptr d = t.get_aspect().template get<_generator_>()(t, i, opt.data_size);
      data_line.push_back(std::move(d));
    }
  }
};

UNIT(init_test, "")
{
  using namespace fas::testing;
  t.get_aspect().template get<_init_line_>()(t);

  auto& data_line = t.get_aspect().template get<_data_line_>();
  auto& test_opt = t.get_aspect().template get<_test_options_>();
  auto& dl_opt = data_line.options();

  bool opt_test = 0 == std::memcmp(&test_opt, &dl_opt, sizeof(dl_opt) ) ;
  t << is_true<assert>( opt_test ) << FAS_TESTING_FILE_LINE;
  t << stop;

  size_t linesize = data_line.size();
  size_t testsize = test_opt.data_size * test_opt.data_count;
  t << is_true<assert>( linesize == testsize) << FAS_TESTING_FILE_LINE;

  linesize = 0;
  size_t linecount = 0;
  size_t initcount = data_line.count();
  auto d = data_line.next(nullptr);
  while ( d != nullptr )
  {
    ++linecount;
    linesize += d->size();
    t << equal<assert>( data_line.size()+linesize, testsize) << FAS_TESTING_FILE_LINE;
    t << equal<assert>( data_line.count()+linecount, initcount) << FAS_TESTING_FILE_LINE;
    t << stop;
    d = data_line.confirm_next();
  }
  t << equal<assert>( data_line.size(), 0) << FAS_TESTING_FILE_LINE;
  t << equal<assert>( data_line.count(), 0) << FAS_TESTING_FILE_LINE;
  t << message("linecount: ") << linecount;
  t << message("linesize: ") << linesize;
}

UNIT(nobuf_test, "non-buffering mode")
{
  using namespace fas::testing;

  auto& data_line = t.get_aspect().template get<_data_line_>();
  auto test_opt = t.get_aspect().template get<_test_options_>();

  data_line.clear();
  data_line.set_options(test_opt);
  auto& dl_opt = data_line.options();

  size_t maxbuf = dl_opt.maxbuf;
  size_t cursize = 1;
  for (size_t c = 0; c < test_opt.data_count; ++c)
  {
    auto d1 = generate(t, c, cursize);
    auto d2 = generate(t, c, cursize);
    auto d3 = data_line.next( std::move(d1) );
    t << is_true<assert>( d3!=nullptr ) << "c: " << c << ": " << FAS_TESTING_FILE_LINE;
    t << stop;
    t << equal<assert>( *d3, *d2 ) << "c: " << c << ", " << *d3 << "!=" << *d2 << FAS_TESTING_FILE_LINE;
    cursize += 1;
    if ( cursize > maxbuf )
      cursize = 1;
    data_line.confirm();
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
  //typedef typename decltype(data_line)::data_ptr  data_ptr;

  auto test_opt = t.get_aspect().template get<_test_options_>();
  test_opt.except_first = false;
  test_opt.except_confirm = false;

  data_line.clear();
  data_line.set_options(test_opt);
  auto& dl_opt = data_line.options();

  size_t maxbuf = dl_opt.maxbuf*2;
  size_t cursize = 1;
  for (size_t c = 0; c < test_opt.data_count; ++c)
  {
    //t << message("c1: ") << c;
    auto d1 = generate(t, 0, cursize); // исходные данные и результат
    auto d2 = generate(t, 0, cursize); // для проверки
    auto d3 = data_line.next( std::move(d1) ); // следующая порция
    while ( d3 != nullptr )
    {
      if ( d1==nullptr )
        d1=std::make_unique<data_type>(d3->begin(), d3->end());
      else
        std::copy(d3->begin(), d3->end(), std::back_inserter(*d1));
      t << message("c: ") << c << " " << *d3;
      d3 = data_line.confirm_next();
    }
    t << equal<assert>( *d1, *d2 ) << "c: " << c << ", " << *d1 << "!=" << *d2 << FAS_TESTING_FILE_LINE;
    cursize += 1;
    if ( cursize > maxbuf )
      cursize = 1;
    t << message("----------------") ;
    t << message(*d1) ;
    

  }
}


BEGIN_SUITE(aux,"aux suite")
  ADD_UNIT(init_test)
  ADD_UNIT(nobuf_test)
  ADD_UNIT(fullbuf_test)
  ADD_VALUE(_test_options_, data_line_test_options )
  ADD_VALUE(_data_line_, ::iow::data_line< std::string > )
  ADD_ADVICE(_generator_, generator)
  ADD_ADVICE(_init_line_, init_line)
END_SUITE(aux)

::fas::testing::suite_counts my_suite_run(int , char*[])
{
  data_line_test_options optlist[]={
  //       bufsize maxbuf minbuf wrnsize maxsize except_first except_confirm data_size data_count
    getopt(0,      0,     0,     0,      0,      false,        false,        10,       10),
    getopt(10,     0,     0,     0,      0,      false,        false,        100,      100),
    getopt(10,     10,    10,    0,      0,      false,        false,        11,       1),
    getopt(10,     20,    5,     0,      0,      false,        false,        11,       33),
    getopt(10,     20,    5,     0,      0,      false,        false,        100,       100),
  };
  std::cout << sizeof(optlist) <<std::endl;
  ::fas::testing::suite_counts sc;
  aux_suite as;
  auto& opt = as.get_aspect().get<_test_options_>();
  for (size_t i =0 ; i < std::extent<decltype(optlist)>::value; i++)
  {
    std::cout << std::endl << i << std::endl;
    opt=optlist[i];
    as.run();
    sc += as.counts();
  }
  return sc;
}


BEGIN_TEST
  //RUN_SUITE(aux)
  RUN_SUITE(my)
END_TEST
