#pragma once

#include <iow/io/acceptor/aspect.hpp>
#include <iow/io/descriptor/holder.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace acceptor{

template <typename ConnectionType, typename A = fas::empty_type>
//using acceptor = ::iow::io::descriptor::holder< typename fas::merge_aspect<A, aspect<ConnectionType> >::type >;
class acceptor
  : public ::iow::io::descriptor::holder< typename fas::merge_aspect<A, aspect<ConnectionType> >::type >
{
  typedef ::iow::io::descriptor::holder< typename fas::merge_aspect<A, aspect<ConnectionType> >::type > super;
public:
  typedef typename super::descriptor_type descriptor_type;
  typedef typename super::mutex_type mutex_type;


  acceptor(descriptor_type&& desc)
    : super( std::forward<descriptor_type>(desc))
  {}

  template<typename O>
  void listen(O&& opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    this->get_aspect().template get<_listen_>()(*this, std::forward<O>(opt) );
  }
};

}}}
