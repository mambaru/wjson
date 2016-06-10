#include "timer.hpp"
#include "bique.hpp"
#include "asio_queue.hpp"
#include "delayed_queue.hpp"
#include "common_timer.hpp"

namespace iow{

std::function<void()> timer::make( std::shared_ptr<bique> pq,        duration_t delay, handler h, bool expires_after, wflag_type wflag )
{
  return common_timer::make(pq, delay, std::move(h), expires_after, wflag);
}

std::function<void()> timer::make( std::shared_ptr<asio_queue> pq,   duration_t delay, handler h, bool expires_after, wflag_type wflag )
{
  return common_timer::make(pq, delay, std::move(h), expires_after, wflag);
}

std::function<void()> timer::make( std::shared_ptr<delayed_queue> pq, duration_t delay, handler h, bool expires_after, wflag_type wflag )
{
  return common_timer::make(pq, delay, std::move(h), expires_after, wflag);
}

std::function<void()> timer::make( std::shared_ptr<bique> pq,        duration_t delay, async_handler h, bool expires_after, wflag_type wflag )
{
  return common_timer::make(pq, delay, std::move(h), expires_after, wflag);
}

std::function<void()> timer::make( std::shared_ptr<asio_queue> pq,   duration_t delay, async_handler h, bool expires_after, wflag_type wflag )
{
  return common_timer::make(pq, delay, std::move(h), expires_after, wflag);
}

std::function<void()> timer::make( std::shared_ptr<delayed_queue> pq, duration_t delay, async_handler h, bool expires_after, wflag_type wflag )
{
  return common_timer::make(pq, delay, std::move(h), expires_after, wflag);
}

bool timer::today_from_string(const std::string& strtime, time_point_t& result)
{
  std::time_t now = std::time(0);
  std::tm ptm;
  ::localtime_r(&now, &ptm);

  if ( nullptr ==  strptime(strtime.c_str(), "%H:%M:%S", &ptm) )
  {
    result = std::chrono::system_clock::now();
    return false;
  }

  time_t day_in_sec = 3600*24;
  time_t beg_day = (now/day_in_sec) * day_in_sec - ptm.tm_gmtoff;
  time_t ready_time_of_day = ptm.tm_sec + ptm.tm_min*60 + ptm.tm_hour*3600;
  time_t ready_time = beg_day + ready_time_of_day;
  if ( ready_time < now )
    ready_time += day_in_sec;

  result = std::chrono::system_clock::from_time_t(ready_time);
  return true;
}


}
