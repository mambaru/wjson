#pragma once

#include <iow/io/aux/item_pool.hpp>

namespace iow{ namespace io{
  
template<typename DataType>
class data_pool
  : public item_pool<DataType>
{
  typedef item_pool<DataType> super;
  typedef typename super::data_type data_type;
  typedef typename super::data_ptr  data_ptr;
  
public:
  
  data_pool()
    : super()
    , _bufsize(0)
  {}
  
  void init(size_t poolsize, size_t bufsize) 
  {
    super::init(poolsize);
    _bufsize = bufsize;
  }
  
  data_ptr create() 
  {
    data_ptr d = super::create();
    if ( d->empty() )
      d->resize(_bufsize);
    return std::move(d);
  }
  
  void free(data_ptr d)
  {
    if ( super::is_full() )
      return;
    
    if ( d->capacity() > _bufsize*2 )
      return;
    
    d->reserve(_bufsize);
    super::free( std::move(d) );
  }
  
private:
  size_t _bufsize;
};

}}
