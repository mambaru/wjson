#pragma once

#include <iow/aux/data_pool.hpp>

namespace iow{ 
  
template<typename DataType>
class buffer_pool
  : public data_pool<DataType>
{
  typedef data_pool<DataType> super;
  typedef typename super::data_type data_type;
  typedef typename super::data_ptr  data_ptr;
  
public:
  
  //typedef DataType data_type;
  //typedef std::unique_ptr<data_type> data_ptr;
  
  buffer_pool()
    : super()
    , _buffsize(0)
  {}
  
  void init(size_t poolsize, size_t buffsize) 
  {
    super::init(poolsize);
    _buffsize = buffsize;
  }
  
  data_ptr create() 
  {
    data_ptr d = super::create();
    if ( d->empty() )
      d->resize(_buffsize);
    return std::move(d);
  }
  
  void free(data_ptr d)
  {
    if ( super::is_full() )
      return;
    
    d->resize(_buffsize);
    d->shrink_to_fit();
    super::free( std::move(d) );
  }
  
private:
  size_t _buffsize;
};

}