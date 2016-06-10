#include "timer_manager_base.hpp"
#include "timer.hpp"
#include <iow/workflow/asio_queue.hpp>
#include <iow/workflow/delayed_queue.hpp>
#include <iow/workflow/bique.hpp>

namespace iow{

timer_manager_base::timer_manager_base()
  : _id_counter(0)
{}

std::shared_ptr<bool> timer_manager_base::detach(timer_id_t id)
{
  std::shared_ptr<bool> res;
  std::lock_guard< mutex_type > lk(_mutex);
  auto itr = _id_map.find(id);
  if ( itr == _id_map.end() )
    return nullptr;
  res = itr->second;
  _id_map.erase(itr);
  return res;
}

bool timer_manager_base::release( timer_id_t id )
{
  std::lock_guard< mutex_type > lk(_mutex);
  auto itr = _id_map.find(id);
  if ( itr == _id_map.end() )
    return false;
  _id_map.erase(itr);
  return true;
}

size_t timer_manager_base::clear()
{
  std::lock_guard< mutex_type > lk(_mutex);
  size_t s = _id_map.size();
  _id_map.clear();
  return s;
}

size_t timer_manager_base::size() const
{
  std::lock_guard< mutex_type > lk(_mutex);
  return _id_map.size();
}

template<typename Q, typename Handler>
timer_manager_base::timer_id_t timer_manager_base::create_( std::shared_ptr<Q> pq,  time_point_t start_time, duration_t delay, Handler handler, bool expires_after)
{
  if ( delay.count() == 0 )
    delay = std::chrono::hours(24);
  timer_id_t id = ++_id_counter;
  std::shared_ptr<bool> pflag = std::make_shared<bool>(true);
  std::weak_ptr<bool> wflag = pflag;
  _id_map.insert( std::make_pair(id, pflag) );
  pq->post_at( start_time, timer::make(pq, delay, std::move(handler), expires_after, wflag) );
  return id;
}


timer_manager_base::timer_id_t timer_manager_base::create( std::shared_ptr<bique> pq,  time_point_t start_time, duration_t delay, handler h, bool expires_after)
{
  std::lock_guard< mutex_type > lk(_mutex);
  return this->create_(pq, start_time, delay, std::move(h), expires_after);
}

timer_manager_base::timer_id_t timer_manager_base::create( std::shared_ptr<bique> pq,  time_point_t start_time, duration_t delay, async_handler h, bool expires_after)
{
  std::lock_guard< mutex_type > lk(_mutex);
  return this->create_(pq, start_time, delay, std::move(h), expires_after);
}

timer_manager_base::timer_id_t timer_manager_base::create( std::shared_ptr<delayed_queue> pq,  time_point_t start_time, duration_t delay, handler h, bool expires_after)
{
  std::lock_guard< mutex_type > lk(_mutex);
  return this->create_(pq, start_time, delay, std::move(h), expires_after);
}

timer_manager_base::timer_id_t timer_manager_base::create( std::shared_ptr<delayed_queue> pq,  time_point_t start_time, duration_t delay, async_handler h, bool expires_after)
{
  std::lock_guard< mutex_type > lk(_mutex);
  return this->create_(pq, start_time, delay, std::move(h), expires_after);
}

timer_manager_base::timer_id_t timer_manager_base::create( std::shared_ptr<asio_queue> pq,  time_point_t start_time, duration_t delay, handler h, bool expires_after)
{
  std::lock_guard< mutex_type > lk(_mutex);
  return this->create_(pq, start_time, delay, std::move(h), expires_after);
}

timer_manager_base::timer_id_t timer_manager_base::create( std::shared_ptr<asio_queue> pq,  time_point_t start_time, duration_t delay, async_handler h, bool expires_after)
{
  std::lock_guard< mutex_type > lk(_mutex);
  return this->create_(pq, start_time, delay, std::move(h), expires_after);
}

}
