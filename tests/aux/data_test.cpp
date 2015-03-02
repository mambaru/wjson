#include <iow/aux/data_line.hpp>
#include <fas/testing.hpp>
#include <algorithm>


struct _data_line_;
struct _generator_;
struct _data_size_;
struct _data_line_init_;

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

template<
  size_t DataSize, 
  size_t DataCount, 
  size_t BufSize, 
  size_t WrnSize,  
  size_t MaxSize
>
struct data_line_init
  : fas::testing::unit
{
public:
  
  data_line_init() 
    : fas::testing::unit("data_line_init", "data_line_init")
  {}
  
  template<typename T>
  void operator()(T& t)
  {
    typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
    typedef typename data_line_t::data_type data_type;
    typedef typename data_line_t::data_ptr data_ptr;
    
    data_line_t& data_line = t.get_aspect().template get<_data_line_>();
    
    data_line.clear();
    data_line.set_options({BufSize, WrnSize, MaxSize});
    
    data_ptr first;
    for (size_t i =0; i < DataCount; i++)
    {
      data_ptr d = std::make_unique<data_type>();
      for (size_t j =0; j < DataCount; j++)
        d->push_back('A'+j%10);
      if (i==0)
        first = data_line.create(std::move(d));
      else
        data_line.create(std::move(d));
    }
    data_line.rollback( std::move(first) );
  }
};



UNIT(data_line_first, "")
{
  using namespace fas::testing;
  typedef typename T::aspect::template advice_cast<_data_line_>::type data_line_t;
  typedef typename data_line_t::data_type data_type;
  typedef typename data_type::value_type value_type;
  typedef typename data_line_t::data_ptr data_ptr;
  data_line_t& data_line = t.get_aspect().template get<_data_line_>();
  size_t counter = 0;
  auto d = data_line.create(nullptr);
  while (d!=nullptr)
  {
    auto dd = t.get_aspect().template get<_generator_>()(t, counter, d->size());
    counter += d->size();
    t << equal<assert>(*dd, *d) << *dd << "!=" << *d << FAS_TESTING_FILE_LINE;
    d = data_line.create(nullptr);
  }
}

template<size_t DataSize, size_t DataCount, size_t BufSize, size_t WrnSize,  size_t MaxSize>
BEGIN_UNIT_LIST(data_line)
  , fas::advice<_data_line_init_, data_line_init<DataSize, DataCount, BufSize, WrnSize, MaxSize> >
  , fas::group< fas::testing::_units_, _data_line_init_>
  ADD_UNIT(data_line_first)
END_UNIT_LIST(data_line)


BEGIN_SUITE(aux,"")
  ADD_UNIT_LIST(data_line)<10, 10,  10, 10, 10>
  ADD_VALUE(_data_line_, ::iow::data_line< std::string > )
  ADD_ADVICE(_generator_, generator)
END_SUITE(aux)

::fas::testing::suite_counts my_suite_run(int , char*[])
{
  ::fas::testing::suite_counts sc;
  aux_suite as;
  for (int i =0 ; i < 10; i++)
  {
    as.run();
    sc += as.counts();
  }
  return sc;
}


BEGIN_TEST
  RUN_SUITE(aux)
  RUN_SUITE(my)
END_TEST
