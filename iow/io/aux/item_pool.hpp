#pragma once

#include <iow/memory.hpp>
#include <vector>
#include <memory>

namespace iow{ namespace io{
  
// TODO: Убрать
template<typename DataType>
class item_pool
{
public:
  
  typedef DataType data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
  item_pool()
    : _poolsize(0)
  {}
  
  void init(size_t poolsize) 
  {
    _poolsize = poolsize;
    _pool.reserve(poolsize);
    _pool.shrink_to_fit();
  }
  
  data_ptr create()
  {
    if ( _pool.empty() )
      return std::make_unique<data_type>();
  
    data_ptr d = std::move(_pool.back());
    _pool.pop_back();
    return std::move(d);
  }
  
  void free(data_ptr d)
  {
    if ( is_full() )
      return;
    _pool.push_back( std::move(d) );
  }
  
  bool is_full() const
  {
    return _pool.size() >= _poolsize;
  }
  
private:
  size_t _poolsize;
  std::vector<data_ptr> _pool;
};

}}
