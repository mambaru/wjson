#include "asio_queue.hpp"

#include <iow/logger/logger.hpp>
#include <iow/system.hpp>
#include <iow/boost.hpp>

namespace iow{
  
asio_queue::asio_queue(io_service_type& io, const size_t maxsize)
  : _io(io)
{
  _counter = 0;
  _maxsize = maxsize;
}
  
void asio_queue::set_maxsize(size_t maxsize)
{
  _maxsize = maxsize;
}

asio_queue::io_service_type& asio_queue::get_io_service() 
{ 
  return _io;
}

const asio_queue::io_service_type& asio_queue::get_io_service() const 
{
  return _io;
}
  
bool asio_queue::run()
{
  return 0!=_io.run();
}
  
bool asio_queue::run_one()
{
  return 0!=_io.run_one();
}
  
bool asio_queue::poll_one()
{
  return 0!=_io.poll_one();
}
  
void asio_queue::reset() {}
  
void asio_queue::stop(){}
  
bool asio_queue::post( function_t f )
{
  if ( !this->check_() )
    return false;  
  std::weak_ptr<self> wthis = this->shared_from_this();
  ++_counter;
  _io.post( [wthis, f]()
  {
    if (auto pthis = wthis.lock() )
    {
      --pthis->_counter;
      f();
    }
  } );
  return true;
}

bool asio_queue::post_at(time_point_t tp, function_t f)
{
  if ( !this->check_() )
    return false;

  auto ptimer = this->create_timer_( tp );
  std::weak_ptr<self> wthis = this->shared_from_this();
  ++_counter;
  ptimer->async_wait([f, ptimer, wthis]( const ::iow::system::error_code& )
  {
    if (auto pthis = wthis.lock() )
    {
      --pthis->_counter;
      f();
    }
  });
  return true;
}
  
bool asio_queue::delayed_post(duration_t duration, function_t f)
{
  return this->post_at( std::move( std::chrono::system_clock::now() + duration  ), std::move(f) );
}
  
std::size_t asio_queue::size() const
{
  return _counter;
}

std::size_t asio_queue::dropped() const
{
  return _drop_count;
}
  
//private:
  
bool asio_queue::check_()
{
  if ( _maxsize == 0 )
    return true;
  if ( _counter < _maxsize )
    return true;
  ++_drop_count;
  return false;
}

template<typename TP>
asio_queue::timer_ptr asio_queue::create_timer_(TP tp)
{
  typedef std::chrono::microseconds duration_t; 
  typedef duration_t::rep rep_t; 
  rep_t d = std::chrono::duration_cast<duration_t>(tp.time_since_epoch()).count(); 
  rep_t sec = d/1000000; 
  rep_t mksec = d%1000000; 
  ::boost::posix_time::ptime ptime = 
    ::boost::posix_time::from_time_t(0)
    + ::boost::posix_time::seconds(static_cast<long>(sec))
    + ::boost::posix_time::microseconds(mksec);
  return std::make_shared<timer_type>( this->_io, ptime);
}

} // iow

