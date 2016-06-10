#include "task_manager.hpp"
#include "timer_manager.hpp"
#include "iow/workflow/bique.hpp"
#include "thread_pool.hpp"
#include <iow/asio.hpp>

namespace iow{

  
class task_manager::pool_impl
  : public thread_pool<task_manager::queue_type>
{
  typedef thread_pool<task_manager::queue_type> super;
public:
  pool_impl(super::service_ptr service)
    : super(service)
  {}
};


task_manager::task_manager( io_service_type& io, const queue_options& opt )
  : _threads(0)
{
  // delayed отключен и пул потоков
  _queue = std::make_shared<queue_type>(io, opt, true );
  _timer = std::make_shared<timer_type>(_queue);
  _pool = nullptr;
}
  
task_manager::task_manager( const queue_options& opt, int threads )
  : _threads(threads)
{
  _queue = std::make_shared<queue_type>(opt);
  _timer = std::make_shared<timer_manager<queue_type> >(_queue);
  _pool = std::make_shared<pool_type>(_queue);
}
  
task_manager::task_manager( io_service_type& io, const queue_options& opt, int threads, bool use_asio /*= false*/  )
  : _threads(threads)
{
  _queue = std::make_shared<queue_type>(io, opt, threads==0 || use_asio );
  _timer = std::make_shared<timer_type>(_queue);
  _pool = std::make_shared<pool_type>(_queue);
}

void task_manager::reconfigure(const queue_options& opt, int threads, bool use_asio /*= false*/ )
{
  _threads = threads;
  _queue->reconfigure(opt, threads==0 || use_asio );
}
  
void task_manager::rate_limit(size_t rps) 
{
  if ( _pool!=nullptr) 
    _pool->rate_limit(rps);
}

void task_manager::start()
{
  if ( _pool!=nullptr) 
    _pool->start(_threads);
}

void task_manager::stop()
{
  _timer->clear();
  _queue->stop();
  if ( _pool!=nullptr) 
    _pool->stop();
  _queue->reset();
}

void task_manager::run()
{
  _queue->run();
}
  
bool task_manager::run_one()
{
  return _queue->run_one();
}
  
bool task_manager::poll_one()
{
  return _queue->poll_one();
}
  
bool task_manager::post( function_t&& f )
{
  return _queue->post(std::move(f) );
}
  
bool task_manager::post_at(time_point_t tp, function_t&& f)
{
  return _queue->post_at( tp, std::move(f));
}

bool task_manager::delayed_post(duration_t duration, function_t&& f)
{
  return _queue->delayed_post(duration, std::move(f));
}
  
std::size_t task_manager::size() const
{
  return _queue->size();
}
  
std::shared_ptr<task_manager::timer_type> task_manager::timer()
{
  return _timer;
}

}
