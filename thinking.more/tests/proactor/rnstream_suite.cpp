
#include <iostream>
#include <boost/concept_check.hpp>
#include <iow/descriptor/descriptor_holder.hpp>
#include <iow/proactor/data/context.hpp>
#include <iow/proactor/data/aspect/aspect.hpp>
#include <iow/proactor/data/stream/aspect.hpp>
#include <iow/proactor/aspect/aspect.hpp>
#include <iow/types.hpp>
#include <iow/aux/asio.hpp>
#include <iow/basic/tags.hpp>
#include <fas/testing.hpp>




class stream_emulator
{
public:
  stream_emulator(::iow::asio::io_service& io)
    : _io(io)
    , _step(0)
    , _pos(0)
  {
  }
  
  void init(const std::string& data, size_t step)
  {
    _data = data;
    _step = step;
    _pos = 0;
  }
  
  typedef std::function< void(const boost::system::error_code&, std::size_t) > read_handler;
  
  void async_read_some( const boost::asio::mutable_buffer& buffers, read_handler callback)
  {
    if ( _step == 0 )
      abort();
    
    const iovec* const_native = reinterpret_cast<const iovec*>(&buffers);
    iovec* native = const_cast<iovec*>(const_native);
    char *data = static_cast<char*>(native->iov_base);
    size_t size = native->iov_len;
    
    _io.post([this, data, size, callback](){
      std::cout << "copy..." << (size_t)(data) << std::endl;
      size_t s = this->next( data, size);
      std::cout << "copy ready..." << (size_t)(data) << std::endl;
      callback( ::boost::system::error_code(), s );
      std::cout << "callback ready..." << (size_t)(data) << std::endl;
      /*
       * std::cout << "copy..." << (size_t)(data) << std::endl;
      memcpy(data, "aaaa\r\n", 6);
      std::cout << "ready..." << std::endl;
      callback( ::boost::system::error_code(), 6 );
      */
    });
  }
  
  size_t next(char *data, size_t size)
  {
    auto beg = _data.begin() + _pos;
    auto end = beg;
    if ( _pos + _step < _data.size() )
    {
      end += _step;
    }
    else
    {
      end = _data.end();
    }
    
    if (std::distance(beg, end) > size )
    {
      end = beg + size;
    }
    
    std::copy(beg, end, data);
    
    if ( end == _data.end() )
      _pos = 0;
    else
      _pos += std::distance(beg, end);
    return std::distance(beg, end);
  }
private:
  iow::asio::io_service& _io;
  std::string _data;
  size_t _step;
  size_t _pos;
};

template<typename A = fas::aspect<> >
class proactor
  : public ::iow::descriptor_holder< A >
  , public std::enable_shared_from_this< proactor<A> >
{
  typedef proactor<A> self;
  typedef ::iow::descriptor_holder< A > super;

public:  
  typedef typename super::data_ptr data_ptr;
  typedef typename super::mutex_type mutex_type;
  
  IOW_INHERITING_CONSTRUCTORS_T(proactor, super)
  IOW_IO_BASE_IMPL_T(super)

  /*
  template<typename T>
  void write(T& t, data_ptr d)
  {
    std::lock_guard<mutex_type> lk(super::mutex());
    super::write_(*this, std::move(d) );
  }
  */
  
public:
  
};

struct rnstream_options{};
struct rnstream_context 
  : iow::proactor::data::context< std::vector<char> > 
{
};

typedef proactor< fas::aspect<
  fas::type< ::iow::_options_type_, rnstream_options>,
  fas::type< ::iow::_context_type_, rnstream_context>,
  fas::type< ::iow::_mutex_type_, std::recursive_mutex>,
  fas::type< ::iow::_descriptor_type_, stream_emulator >,
  ::iow::proactor::data::stream::advice_list,
  ::iow::proactor::data::advice_list,
  ::iow::proactor::advice_list
> > rnstream_echo_base;

class rnstream_echo: public rnstream_echo_base{
public:
  IOW_INHERITING_CONSTRUCTORS(rnstream_echo, rnstream_echo_base)
};



UNIT(rn_echo, "")
{
  using namespace fas::testing;
  typedef rnstream_echo conn_type;
  typedef std::shared_ptr<conn_type> conn_ptr;
  typedef conn_type::io_service_type io_service_type;
  typedef conn_type::options_type options_type;
  io_service_type io;
  options_type opt;
  auto conn=std::make_shared<conn_type>(io,opt);
  conn->descriptor().init( "abcde", 2 );
  conn->context_().status = true;
  std::cout << "start1 " << conn->context_().status << std::endl;
  conn->start();
  std::cout << "start2 " << conn->context_().status << std::endl;
  io_service_type::work wrk(io);
  conn->context_().status = true;
  io.run_one();
  io.run_one();
  io.run_one();
  io.run_one();
  io.run_one();
  //posix::stream_descriptor in(my_io_service, ::dup(STDIN_FILENO));
  
  t << nothing;
}

BEGIN_SUITE(rnstream,"")
  ADD_UNIT(rn_echo)
END_SUITE(rnstream)

