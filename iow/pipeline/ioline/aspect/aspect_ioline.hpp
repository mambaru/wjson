#pragma once

#include <iow/pipeline/ioline/aspect/tags.hpp>
#include <fas/aop.hpp>
#include <memory>


namespace iow{
  


struct ad_make_descriptor
{
  template<typename T>
  auto operator()(T& t)
    -> typename T::data_ptr
  {
    typedef typename T::data_type descriptor_type;
    return std::make_shared<descriptor_type>( t.get_io_service(), t.options_().descriptor );
    /*
    std::cout << "Make descriptor..." << std::endl;
    typedef typename T::data_type descriptor_type;
    typedef typename T::data_ptr  descriptor_ptr;
    descriptor_ptr d( new descriptor_type( t.get_io_service(), t.options_().descriptor ) );
    return d;
    */
  }
};

  
typedef fas::aspect<
  fas::advice<_make_data_, ad_make_descriptor>
> aspect_ioline;

}
