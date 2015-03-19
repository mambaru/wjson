#include <iostream>
#include <iow/io/io_base.hpp>
#include <iow/io/basic/aspect.hpp>
#include <iow/io/flow/aspect.hpp>
#include <iow/io/pipe/aspect.hpp>
#include <iow/memory.hpp>
#include <iow/asio.hpp>

#include <fas/testing.hpp>

#include <vector>
#include <memory>
#include <list>
#include <cstdlib>
typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

struct ad_read_some
{
  template<typename T>
  void operator()(T& t, typename T::input_t d)
  {
    auto dd = std::make_shared<typename T::input_t>( std::move(d) );
    t.service.post([&t, dd](){
      auto tmp = std::move(t.input.front());
      t.input.pop_front();
      std::copy(tmp->begin(), tmp->end(), (*dd)->begin());
      (*dd)->resize(tmp->size());
      t.get_aspect().template get< ::iow::io::flow::_complete_>()(t, std::move(*dd));
    });
  }
};

struct ad_write_some
{
  template<typename T>
  void operator()(T& t, typename T::input_t d)
  {
    auto dd = std::make_shared<typename T::input_t>( std::move(d) );
    t.service.post([&t, dd](){
      std::cout << "pipe write" << std::endl;
      t.result += std::string((*dd)->begin(), (*dd)->end());
      t.get_aspect().template get< ::iow::io::pipe::_complete_>()(t, std::move(*dd), (*dd)->size() );
    });
  }
};

struct ad_input_factory
{
  template<typename T>
  typename T::input_t operator()(T& t)
  {
    if ( t.input.empty() )
      return nullptr;
    return std::make_unique<data_type>(100);
  }
};

struct ad_output_factory
{
  template<typename T>
  typename T::input_t operator()(T& , typename T::output_t d, size_t size)
  {
    if ( size == d->size() )
      return nullptr;

    if ( size == 0 )
      return std::move(d);

    std::abort();
  }
};



class pipe1
  : public ::iow::io::io_base< fas::aspect< 
      ::iow::io::basic::aspect<>::advice_list,
      ::iow::io::flow::aspect< ::iow::io::pipe::_output_, data_ptr >::advice_list,
      ::iow::io::pipe::aspect<  data_ptr >::advice_list,
      fas::advice< ::iow::io::flow::_factory_, ad_input_factory>,
      fas::advice< ::iow::io::pipe::_factory_, ad_output_factory>,
      fas::advice< ::iow::io::flow::_some_, ad_read_some>,
      fas::advice< ::iow::io::pipe::_some_, ad_write_some>
    > >
{
  
public:
  typedef data_ptr input_t;
  typedef data_ptr output_t;

  pipe1(::iow::asio::io_service& io)
    : service(io) 
  {}
  
  void start() 
  {
    this->start_(*this, fas::empty_type() ); 
  }
  
  void add(std::string val)
  {
    input.push_back( std::make_unique<data_type>(val.begin(), val.end()) );
  }
  
  ::iow::asio::io_service& service;
  std::list<data_ptr> input;
  std::string result;
};

UNIT(pipe1, "")
{
  using namespace fas::testing;
  
  ::iow::asio::io_service io;
  pipe1 f(io);
  
  f.add("Hello ");
  f.add("world");
  f.add("!");
  f.start();
  io.run();
  t << equal<expect, std::string>(f.result, "Hello world!") << f.result << FAS_TESTING_FILE_LINE;
  
}

BEGIN_SUITE(pipe,"")
  ADD_UNIT(pipe1)
END_SUITE(pipe)

