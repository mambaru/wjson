#pragma once

#include <iow/aux/memory.hpp>
#include <vector>
#include <memory>

namespace iow{ 
  
template<typename DataType>
class data_pool
{
public:
  
  typedef DataType data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
  data_pool()
    : _poolsize(0)
  {}
  
  void init(size_t poolsize) 
  {
    _poolsize = poolsize;
    _pool.resize(poolsize);
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
    _pool.push_back(d);
  }
  
  bool is_full() const
  {
    return _pool.size() >= _poolsize;
  }
  
private:
  size_t _poolsize;
  std::vector<data_ptr> _pool;
};

}
