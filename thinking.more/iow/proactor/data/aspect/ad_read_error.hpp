#pragma once

//#include <boost/asio.hpp>
#include <iow/aux/system.hpp>

namespace iow{ namespace proactor{
  
struct ad_read_error
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, ::iow::system::error_code ec)
  {
    /*
    t.get_aspect().template get<_status_ >() = false;
    
    if ( ec == boost::asio::error::operation_aborted)
    {
      t.get_aspect().template gete< _on_abort_ >()(t, std::ref(ec) );
    }
    else
    {
      t.get_aspect().template gete< _on_error_ >()(t, std::ref(ec) );
    }
    
    t.get_aspect().template get< _free_buffer_ >()(t, std::move(d));
    */
  }
};

}}
