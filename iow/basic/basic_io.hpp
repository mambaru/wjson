#pragma once


#include <wfc/io/io_base.hpp>
#include <wfc/io/basic/aspect.hpp>

#include <fas/aop.hpp>

namespace wfc{ namespace io{ 

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class basic_io
  : public io_base< typename fas::merge_aspect<A, basic::aspect>::type, AspectClass> 
{
public:
  typedef basic_io<A, AspectClass> self;
  typedef io_base< typename fas::merge_aspect<A, basic::aspect>::type, AspectClass> super;
  
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  basic_io(io_service_type& io_service, const options_type& opt =  options_type() )
    : super( io_service, opt)
  {
  }
};


}}
