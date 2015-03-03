#include <iow/aux/data_line.hpp>
#include <fas/testing.hpp>
#include <algorithm>


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
  operator()(T&, size_t counter, size_t size) 
  {
    typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
    typedef typename data_line_t::data_type data_type;
    typedef typename data_line_t::data_ptr data_ptr;
    
    data_ptr d = std::make_unique<data_type>();
    for ( size_t j =counter; j < counter + size; j++)
      d->push_back('A'+j%10);
    //d->push_back('A');
    return std::move(d);
  }
};

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
      /*if (i==0)
        first = data_line.next(std::move(d));
      else
        data_line.next(std::move(d));
      */
    }
    //data_line.rollback( std::move(first) );
  }
};

UNIT(data_line_first, "")
{
  using namespace fas::testing;
  
  typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
  t.get_aspect().template get<_init_line_>()(t);
  data_line_t& data_line = t.get_aspect().template get<_data_line_>();
  auto& opt = t.get_aspect().template get<_test_options_>();
  size_t counter = 0;
  auto d = data_line.next(nullptr);
  while (d!=nullptr)
  {
    auto dd = t.get_aspect().template get<_generator_>()(t, counter, d->size());
    counter ++;
    t << equal<assert>(d->size(), opt.bufsize) << d->size() << "!=" << opt.bufsize <<": "<< FAS_TESTING_FILE_LINE;
    t << equal<assert>(*dd, *d) << *dd << "!=" << *d <<": "<< FAS_TESTING_FILE_LINE;
    std::cout<< std::endl<< *dd << "==" << *d << std::endl;
    d = data_line.next(nullptr);
  }
}


BEGIN_SUITE(aux,"aux suite")
  ADD_UNIT(data_line_first)
  ADD_VALUE(_test_options_, data_line_test_options )
  ADD_VALUE(_data_line_, ::iow::data_line< std::string > )
  ADD_ADVICE(_generator_, generator)
  ADD_ADVICE(_init_line_, init_line)
END_SUITE(aux)

::fas::testing::suite_counts my_suite_run(int , char*[])
{
  data_line_test_options optlist[]={
  //       bufsize maxbuf minbuf wrnsize maxsize except_first except_confirm data_size data_count
    getopt(0,      0,     0,     0,      0,      true,        true,          10,       10),
    getopt(0,      0,     0,     0,      0,      true,        true,          10,       10)
  };
  std::cout << sizeof(optlist) <<std::endl;
  ::fas::testing::suite_counts sc;
  aux_suite as;
  auto& opt = as.get_aspect().get<_test_options_>();
  for (size_t i =0 ; i < std::extent<decltype(optlist)>::value; i++)
  {
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
