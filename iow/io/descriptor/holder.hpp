#pragma once
#include <iow/io/io_base.hpp>

namespace iow{ namespace io{ namespace descriptor{

template<typename A>
class holder
  : public io_base<A>
{
private:
  typedef holder_base<A> self;
  typedef io_base<A> super;
 
  using super::reset;
  using super::start;
  using super::initialize;
  using super::stop;
  using super::shutdown;

public:
  typedef holder_base<A> self;
  typedef io_base<A> super;
  
  typedef typename super::mutex_type mutex_type;
  typedef typename super::aspect::template advice_cast<_descriptor_type_>::type descriptor_type;

public:
  
  holder_base(descriptor_type&& desc)
    : super()
    , _descriptor( std::forward<descriptor_type>(desc))
  {}
  
  void attach(descriptor_type&& desc)
  {
    _descriptor = std::forward<descriptor_type>(desc);
  }

  descriptor_type&& detach()
  {
    return std::move(_descriptor);
  }

  const descriptor_type& descriptor() const { return _descriptor;}
  
  descriptor_type& descriptor() { return _descriptor;}
  
  void reset()
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::reset_(*this);
  }

  template<typename O>
  void start(O&& opt)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::start_(*this, std::forward<O>(opt));
  }

  template<typename O>
  void initialize(O&& opt)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::initialize_(*this, std::forward<O>(opt));
  }

  void stop()
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::stop_(*this);
  }

  template<typename Handler>
  void shutdown(Handler&& handler)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::shutdown_(*this, std::forward<Handler>(handler));
  }

private:
  descriptor_type _descriptor;
};

}}}

