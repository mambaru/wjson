#include "bique.hpp"
#include "asio_queue.hpp"
#include "delayed_queue.hpp"

namespace iow{
 
bique::~bique()
{
  this->stop();
}
 
bique::bique( const queue_options& opt )
{
  _dflag = true;
  _delayed = std::make_shared<delayed_queue>(opt);
  _timed  = nullptr;
}

bique::bique( io_service_type& io, const queue_options& opt, bool timed /*= true*/ )
{
  _dflag = !timed;
  _delayed = std::make_shared<delayed_queue>(opt);
  _timed  = std::make_shared<asio_queue>(io, opt);
}
  
void bique::reconfigure(const queue_options& opt, bool timed /*= true*/)
{
  bool newflag = !timed;
  if ( _timed == nullptr ) 
    newflag = true;

  _dflag = newflag;
  _delayed->reconfigure(opt);
  if( _timed ) 
    _timed->reconfigure(opt);
}

void bique::reset()
{
  return this->invoke_( &delayed_queue::reset, &asio_queue::reset);
}

bool bique::run()
{
  return this->invoke_( &delayed_queue::run, &asio_queue::run);
}
  
bool bique::run_one()
{
  return this->invoke_( &delayed_queue::run_one, &asio_queue::run_one);
}
  
bool bique::poll_one()
{
  return this->invoke_( &delayed_queue::poll_one, &asio_queue::poll_one);
}

void bique::stop()
{
  return this->invoke_( &delayed_queue::stop, &asio_queue::stop);
}

bool bique::post( function_t f )
{
  return this->invoke_( &delayed_queue::post, &asio_queue::post, std::move(f) );
}

bool bique::post_at(time_point_t tp, function_t&& f)
{
  return this->invoke_( &delayed_queue::post_at, &asio_queue::post_at, tp, std::move(f));
}

bool bique::delayed_post(duration_t duration, function_t&& f)
{
  return this->invoke_( &delayed_queue::delayed_post, &asio_queue::delayed_post, duration, std::move(f));
}
  
std::size_t bique::size() const
{
  return this->invoke_( &delayed_queue::size, &asio_queue::size);
}
  
//private:
  
template<typename R, typename... Args>
R bique::invoke_( 
  R(delayed_queue::* method1)(Args...),
  R(asio_queue::* method2)(Args...), 
  Args... args)
{
  return _dflag 
         ? (_delayed.get()->*method1)( std::move(args)...)
         : (_timed.get()->*method2)( std::move(args)...);
}

template<typename R, typename... Args>
R bique::invoke_( 
  R(delayed_queue::* method1)(Args...) const,
  R(asio_queue::* method2)(Args...) const, 
  Args... args) const
{
  return _dflag 
         ? (_delayed.get()->*method1)( std::move(args)...)
         : (_timed.get()->*method2)( std::move(args)...);
}

}
