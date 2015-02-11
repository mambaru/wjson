#pragma once


#include <iow/basic/io_base.hpp>
#include <iow/basic/aspect_basic.hpp>

namespace iow{ 
  
template<typename A = fas::empty_type >
class basic_io final
  : public io_base< typename fas::merge_aspect<A, aspect_basic>::type >
{
  
public:
  typedef io_base< typename fas::merge_aspect<A, aspect_basic>::type > super;
  typedef typename super::options_type options_type;
  typedef typename super::mutex_type mutex_type; 
  
  IOW_INHERITING_CONSTRUCTORS_T(basic_io, super)
  IOW_IO_BASE_IMPL_T(super)
};

}
