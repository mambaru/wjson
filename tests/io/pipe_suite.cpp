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
#include <boost/concept_check.hpp>
typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

struct ad_read_some
{
  template<typename T>
  void operator()(T& t, typename T::input_t d)
  {
    if ( t.input.empty() )
      return;
    
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
  template<typename T, typename P>
  void operator()(T& t, P p/*, size_t size*/)
  {
    if ( p.first == nullptr )
      return;
    
    t.service.post([&t, p](){
      std::cout << "pipe write" << std::endl;
      t.result += std::string(p.first, p.first + p.second);
      t.get_aspect().template get< ::iow::io::pipe::_complete_>()(t, std::move(p) /*.first, p.second*/ );
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

struct ad_entry
{
  template<typename T>
  void operator()(T& , typename T::output_t d)
  {
    data = std::move(d);
  }
  data_ptr data;
};

struct ad_free
{
  template<typename T>
  void operator()(T& , typename T::output_t )
  {
  }
};

struct ad_confirm
{
  template<typename T, typename D>
  void operator()(T& t, D /*, size_t*/ )
  {
    auto &d = t.get_aspect().template get< ::iow::io::pipe::_attach_ >().data;
    d.reset();
    //return std::move(d);
  }
};

struct ad_next
{
  template<typename T>
  std::pair<const char*, size_t> operator()(T& t)
  {
    auto &d = t.get_aspect().template get< ::iow::io::pipe::_attach_ >().data;
    if ( d == nullptr )
      return std::pair<const char*, size_t>(nullptr, 0);
    return std::make_pair( &((*d)[0]), d->size() );
  }
};



class pipe1
  : public ::iow::io::io_base< fas::aspect< 
      ::iow::io::basic::aspect<>::advice_list,
      ::iow::io::flow::aspect::advice_list,
      ::iow::io::pipe::aspect::advice_list,
      fas::alias< ::iow::io::flow::_confirm_, ::iow::io::pipe::_output_>,
      fas::advice< ::iow::io::flow::_create_, ad_input_factory>,
      fas::advice< ::iow::io::pipe::_attach_, ad_entry>,
      fas::advice< ::iow::io::pipe::_next_, ad_next>,
      //fas::advice< ::iow::io::pipe::_free_, ad_free>,
      fas::advice< ::iow::io::pipe::_confirm_, ad_confirm>,
      fas::advice< ::iow::io::flow::_some_, ad_read_some>,
      fas::advice< ::iow::io::pipe::_some_, ad_write_some>,
      fas::stub<::iow::io::flow::_handler_>
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

