#pragma once

#include <iow/proactor/aspect/tags.hpp>
#include <iow/proactor/data/aspect/tags.hpp>
#include <iow/aux/system.hpp>

namespace iow{ namespace proactor{
  
struct ad_read_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, ::iow::system::error_code ec , std::size_t bytes_transferred)
  {
    std::cout << "ad_read_handler 1" << std::endl;
    if ( !t.status() )
      return;
            
    if ( !ec )
    {
      std::cout << "ad_read_handler 2 : " << bytes_transferred << std::endl;
      d->resize(bytes_transferred);
      std::cout << "ad_read_handler 3" << std::endl;
      t.get_aspect().template get<_ready_>()( t, std::move(d) );
      std::cout << "ad_read_handler 4" << std::endl;
    }
    else
    {
      t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
    }
  }
};

}}
