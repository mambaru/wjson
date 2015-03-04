#pragma once


#include <iow/basic/io_base.hpp>
#include <iow/basic/io_strand_base.hpp>
#include <iow/basic/aspect_basic.hpp>

namespace iow{ 
  
template<typename A = fas::empty_type >
class basic_io 
  : public io_base< typename fas::merge_aspect<A, aspect_basic>::type >
{
  
public:
  typedef io_base< typename fas::merge_aspect<A, aspect_basic>::type > super;
  typedef typename super::options_type options_type;
  typedef typename super::mutex_type mutex_type; 
  
  IOW_INHERITING_CONSTRUCTORS_T(basic_io, super)
  IOW_IO_BASE_IMPL_T(super)
};

template<typename A = fas::empty_type >
class basic_io_strand final
  : public io_strand_base< typename fas::merge_aspect<A, aspect_basic>::type >
{
  
public:
  typedef io_strand_base< typename fas::merge_aspect<A, aspect_basic>::type > super;
  typedef typename super::options_type options_type;
  typedef typename super::mutex_type mutex_type; 
  
  IOW_INHERITING_CONSTRUCTORS_T(basic_io_strand, super)
  IOW_IO_BASE_IMPL_T(super)
};

}
