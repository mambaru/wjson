#pragma once
#include <boost/concept_check.hpp>
#include <mutex>

namespace iow{ 
  
struct ad_read_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    if ( t.incoming_handler_ != nullptr )
    {
      outgoung_handler_t callback = t.outgoung_handler_ ;
      
      if ( callback == nullptr )
      {
        weak_ptr<T> wthis = t.shared_from_this();
        callback = t.wrap_([wthis](typename T::data_ptr d) 
        {
          if (auto pthis = wthis.lock() )
          {
            std::lock_guard<typename T::mutex_type> lk(pthis->mutex);
            if ( pthis->status() )
            {
              pthis->get_aspect().template get<_incoming_>()( std::move(d) );
            }
          }
        });
      }
      t.incoming_handler_(t.get_id(), std::move(d),  )
    }
    else
    {
      t.get_aspect().template get<_incoming_>()( std::move(d) );
    }
  }
};

}