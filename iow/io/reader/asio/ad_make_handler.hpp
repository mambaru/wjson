#pragma once

#include <iow/io/reader/asio/tags.hpp>
#include <iow/logger/logger.hpp>



#include <thread>
namespace iow{ namespace io{ namespace reader{ namespace asio{

struct ad_make_handler
{
  typedef std::function<void(::iow::system::error_code, std::size_t)> handler_type;
  
  template<typename T, typename P>
  handler_type operator()(T& t, const P& p) 
  {
    IOW_LOG_DEBUG("----> ad_make_handler -1-")
    std::weak_ptr<T> wthis = t.shared_from_this();
    return t.wrap([wthis, p]( ::iow::system::error_code ec , std::size_t bytes_transferred )
    {
      IOW_LOG_DEBUG("----> ad_make_handler -2-")
      if ( auto pthis = wthis.lock() )
      {
        IOW_LOG_DEBUG("----> ad_make_handler -3-")
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        IOW_LOG_DEBUG("----> ad_make_handler -4-")
        pthis->get_aspect().template get<_read_handler_>()(*pthis, std::move(p), std::move(ec), bytes_transferred);
        IOW_LOG_DEBUG("----> ad_make_handler -5-")
      }
    });
  }
};

}}}}
