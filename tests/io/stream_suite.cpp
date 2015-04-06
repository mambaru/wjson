#include <iostream>
#include <iow/io/io_base.hpp>
#include <iow/io/basic/aspect.hpp>
#include <iow/io/stream/aspect.hpp>
/*
 * #include <iow/io/flow/aspect.hpp>
#include <iow/io/pipe/aspect.hpp>
*/
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
  template<typename T, typename D>
  void operator()(T& t, D d)
  {
    std::cout << "ad_read_some: " << d.second << std::endl;
    if ( t.input.empty() )
      return;
    
    //auto dd = std::make_shared<typename T::input_t>( std::move(d) );
    
    t.service.post([&t, d](){
       std::cout << "ad_read_some (post ready) : " << d.second << std::endl;
      auto dd = d;
      auto tmp = std::move(t.input.front());
      t.input.pop_front();
      /*std::copy(tmp->begin(), tmp->end(), (*dd)->begin());
      (*dd)->resize(tmp->size());
    */
      std::copy(tmp->begin(), tmp->end(), dd.first);
      dd.second = tmp->size();
      
      t.get_aspect().template get< ::iow::io::flow::_complete_>()(t, std::move(dd));
    });
  }
};

struct ad_write_some
{
  template<typename T, typename P>
  void operator()(T& t, P p)
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

struct stream_options
{
  typedef ::iow::io::data_pool< data_type > buffer_pool_type;
  typedef std::shared_ptr<buffer_pool_type> buffer_pool_ptr;
  typedef ::iow::io::write_buffer_options<data_type> write_buffer_options;
  typedef ::iow::io::read_buffer_options<data_type> read_buffer_options;
  write_buffer_options write_buffer;
  read_buffer_options read_buffer;
  buffer_pool_ptr buffer_pool;
};


class stream
  : public ::iow::io::io_base< fas::aspect< 
      ::iow::io::basic::aspect<>::advice_list,
      ::iow::io::stream::aspect<data_type>::advice_list,
      fas::alias< ::iow::io::stream::_handler_, ::iow::io::pipe::_output_>,
      fas::advice< ::iow::io::flow::_some_, ad_read_some>,
      fas::advice< ::iow::io::pipe::_some_, ad_write_some>
    > >
{
public:
  typedef stream_options options_type;
  typedef data_ptr input_t;
  typedef data_ptr output_t;

  stream(::iow::asio::io_service& io)
    : service(io) 
  {}
  
  void start() 
  {
    stream_options opt;
    opt.buffer_pool = std::make_shared<stream_options::buffer_pool_type>();
    ::iow::io::data_pool_options dpo;
    dpo.poolsize = 10;
    dpo.minbuf = 128;
    dpo.maxbuf = 1024;
    opt.buffer_pool->set_options(dpo);
    //opt.write_buffer = std::make_shared<stream_options::write_buffer_options>();
    //opt.write_buffer->fix();
    //opt.read_buffer = std::make_shared<stream_options::read_buffer_options>();
    this->start_(*this, opt ); 
  }
  
  void add(std::string val)
  {
    input.push_back( std::make_unique<data_type>(val.begin(), val.end()) );
  }
  
  ::iow::asio::io_service& service;
  std::list<data_ptr> input;
  std::string result;
};

UNIT(stream, "")
{
  using namespace fas::testing;
  
  ::iow::asio::io_service io;
  stream f(io);
  
  f.add("Hello ");
  f.add("world");
  f.add("!");
  f.start();
  io.run();
  t << equal<expect, std::string>(f.result, "Hello world!") << f.result << FAS_TESTING_FILE_LINE;
  
}

BEGIN_SUITE(stream,"")
  ADD_UNIT(stream)
END_SUITE(stream)

