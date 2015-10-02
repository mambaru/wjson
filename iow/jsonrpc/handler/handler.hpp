#pragma once

#include <iow/jsonrpc/handler/aspect/tags.hpp>
#include <iow/jsonrpc/errors.hpp>
#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <iow/jsonrpc/types.hpp>
#include <iow/jsonrpc/method/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <mutex>

namespace iow{ namespace jsonrpc{

struct f_get_methods
{
  std::vector<std::string> result;
  
  template<typename T, typename Tg>
  void operator()(T& t, fas::tag<Tg> )
  {
    result.push_back( t.get_aspect().template get<Tg>().name() );
  }
};


template<typename MethodList>
class handler
  : public MethodList
  , public std::enable_shared_from_this< handler<MethodList> >
{
public:
  typedef handler<MethodList> self;
  typedef MethodList super;
  typedef typename super::interface_type interface_type;
  typedef typename super::target_type target_type;
  typedef typename super::peeper_type peeper_type;
  typedef typename super::context_type context_type;
  typedef typename super::options_type options_type;
  
  //typedef typename super::holder_type holder_type;
  typedef typename super::io_id_t io_id_t;
  typedef typename super::outgoing_handler_t outgoing_handler_t;
  typedef typename super::mutex_type mutex_type;
  
  target_type target() const
  {
    return this->get_aspect().template get<_target_>();
  }

  peeper_type peeper() const
  {
    return this->get_aspect().template get<_peeper_>();
  }
  
  context_type& context()
  {
    return this->get_aspect().template get<_context_>();
  }

  const context_type& context() const
  {
    return this->get_aspect().template get<_context_>();
  }
  
  std::vector<std::string> get_methods() const
  {
    // TODO: в аспект
    return fas::for_each_group<_method_>(*this, f_get_methods() ).result;
  }

  void invoke(incoming_holder holder, outgoing_handler_t outgoing_handler) 
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::get_aspect().template get<_invoke_>()(*this, std::move(holder), std::move(outgoing_handler) );
  }

  template<typename O>
  void start(O&& opt)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::start_(*this, std::forward<O>(opt));
    this->get_aspect().template get<_startup_>()(*this, super::get_id() );
  }
  
  void stop()
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    this->get_aspect().template get<_shutdown_>()(*this, super::get_id() );
    super::stop_(*this);
  }

  template<typename O>
  void reconfigure(O&& opt)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::reconfigure_(*this, std::forward<O>(opt));
  }

};

}} // iow

