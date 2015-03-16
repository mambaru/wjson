#pragma once

#include <iow/aux/system.hpp>
#include <memory>
#include <iow/proactor/data/aspect/tags.hpp>

namespace iow{ namespace proactor{ 

struct ad_async_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "ad_async_read_some 1 = " << t.context_().status << std::endl;
    
    if ( !t.status() )
      return;
    
    std::cout << "ad_async_read_some 2" << std::endl;
    
    if ( d == nullptr )
      return;
    
    d->resize(10); // Ахтунг
    std::cout << "ad_async_read_some 3: " << d->size()  << std::endl;
    
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    std::weak_ptr<T> wthis = t.shared_from_this();
    
    auto callback = t.owner_wrap_([wthis, dd]( ::iow::system::error_code ec , std::size_t bytes_transferred )
    { 
      std::cout << "ad_async_read_some::callback begin" << std::endl;
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk( pthis->mutex() );
        std::cout << "ad_async_read_some::callback lock" << std::endl;
        pthis->get_aspect().template get<_read_handler_>()(*pthis, std::move(*dd), std::move(ec), bytes_transferred);
      }
      std::cout << "ad_async_read_some::callback end" << std::endl;
    });
    
    t.descriptor().async_read_some( ::boost::asio::buffer( **dd ), callback);
  }
};

}}
