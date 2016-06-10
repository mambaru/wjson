#include <wfc/core/workflow.hpp>
#include <iow/workflow/workflow.hpp>
#include <iow/workflow/bique.hpp>

namespace iow{
  

workflow::~workflow()
{
  this->stop();
  _impl = nullptr;
}

workflow::workflow(workflow_options opt )
{
  _impl = std::make_shared<task_manager>(opt, opt.threads);
  _impl->rate_limit( opt.rate_limit );
  _delay_ms = opt.post_delay_ms;
}

workflow::workflow(io_service_type& io, workflow_options opt)
{
  _impl = std::make_shared<task_manager>(io, opt, opt.threads, opt.use_io_service);
  _impl->rate_limit( opt.rate_limit );
  _delay_ms = opt.post_delay_ms;
}

void workflow::start()
{
  _impl->start();
}

void workflow::reconfigure(workflow_options opt)
{
  _impl->rate_limit( opt.rate_limit );
  _impl->reconfigure(opt, opt.threads, opt.use_io_service);
  
}

void workflow::stop()
{
  _impl->stop();
}

std::shared_ptr< task_manager > workflow::get() const
{
  return _impl;
}

std::shared_ptr< workflow::timer_type> workflow::get_timer() const
{
  return _impl->timer();
}

bool workflow::post(post_handler handler)
{
  if ( _delay_ms == 0)
    return _impl->post( std::move(handler) );
  else
    return this->post( std::chrono::milliseconds(_delay_ms), std::move(handler) );
}

bool workflow::post(time_point_t tp, post_handler handler)
{
  return _impl->post_at( tp, std::move(handler) );
}

bool workflow::post(duration_t d,   post_handler handler)
{
  return _impl->delayed_post(d, std::move(handler) );
}

workflow::timer_id_t workflow::create_timer(duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_async_timer(duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(duration_t sd, duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create( sd, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_async_timer(duration_t sd, duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create( sd, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(time_point_t tp, duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_async_timer(time_point_t tp, duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_timer(std::string tp, duration_t d, timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

workflow::timer_id_t workflow::create_async_timer(std::string tp, duration_t d, async_timer_handler handler, bool expires_after)
{
  return _impl->timer()->create(tp, d, std::move(handler), expires_after );
}

std::shared_ptr<bool> workflow::detach_timer(timer_id_t id)
{
  return _impl->timer()->detach(id);
}

bool workflow::release_timer( timer_id_t id )
{
  return _impl->timer()->release(id);
}
  
size_t workflow::timer_size() const
{
  return _impl->timer()->size();
}

size_t workflow::queue_size() const
{
  return _impl->size();
}
  
}
