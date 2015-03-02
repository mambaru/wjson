#include <iow/aux/data_line.hpp>
#include <fas/testing.hpp>
#include <algorithm>


struct _data_line_;
struct _generator_;
struct _data_size_;
struct _data_line_init_;
struct _test_options_;

struct data_line_test_options
{
  ::iow::data_line_options dlo;
  size_t data_size;
  size_t data_count;
};

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

UNIT(data_line_init, "")
{
  auto& data_line = t.get_aspect().template get<_data_line_>();
  typedef typename std::remove_reference<decltype(data_line)>::type data_line_t;
  //typedef typename data_line_t::data_type data_type;
  typedef typename data_line_t::data_ptr data_ptr;

  auto& opt = t.get_aspect().template get<_test_options_>();

  data_line.clear();
  data_line.set_options(opt.dlo);

  data_ptr first;
  for (size_t i =0; i < opt.data_count; i++)
  {
    data_ptr d = t.get_aspect().template get<_generator_>()(t, i, opt.data_size);
    if (i==0)
      first = data_line.create(std::move(d));
    else
      data_line.create(std::move(d));
   }
   data_line.rollback( std::move(first) );
}

UNIT(data_line_first, "")
{
  using namespace fas::testing;
  typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
  data_line_t& data_line = t.get_aspect().template get<_data_line_>();
  auto& opt = t.get_aspect().template get<_test_options_>();

  size_t counter = 0;
  auto d = data_line.create(nullptr);
  while (d!=nullptr)
  {
    auto dd = t.get_aspect().template get<_generator_>()(t, counter, d->size());
    counter ++;
    t << equal<assert>(d->size(), opt.dlo.bufsize) << d->size() << "!=" << opt.dlo.bufsize <<": "<< FAS_TESTING_FILE_LINE;
    t << equal<assert>(*dd, *d) << *dd << "!=" << *d <<": "<< FAS_TESTING_FILE_LINE;
    std::cout<< std::endl<< *dd << "==" << *d << std::endl;
    d = data_line.create(nullptr);
  }
}


BEGIN_SUITE(aux,"aux suite")
  ADD_UNIT(data_line_init)
  ADD_UNIT(data_line_first)
  ADD_VALUE(_test_options_, data_line_test_options )
  ADD_VALUE(_data_line_, ::iow::data_line< std::string > )
  ADD_ADVICE(_generator_, generator)
END_SUITE(aux)

::fas::testing::suite_counts my_suite_run(int , char*[])
{
  data_line_test_options optlist[]={
    { {3,10,10}, 10,10},
    { {10,10,10}, 10,10}
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
