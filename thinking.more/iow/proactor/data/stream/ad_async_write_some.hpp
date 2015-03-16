#pragma once

#include <wfc/io/writer/aspect/tags.hpp>
#include <boost/system/error_code.hpp>
#include <memory>

namespace wfc{ namespace io{ namespace writer{
  
struct ad_async_write_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    if ( !t.status() )
      return;
    
    if ( d == nullptr || d->empty() )
      return;

    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    auto pthis = t.shared_from_this();
    auto transfer_size = t.options().output_buffer_size;
    auto callback = [pthis, dd]( boost::system::error_code ec , std::size_t bytes_transferred )
    { 
      typename T::lock_guard lk(pthis->mutex());
      pthis->get_aspect().template get<_write_handler_>()(*pthis, std::move(*dd), std::move(ec), bytes_transferred);
    };
    t.descriptor().async_write_some( ::boost::asio::buffer( **dd, transfer_size ), callback);
  }
};

}}}
