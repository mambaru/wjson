#pragma once

//#include <iow/io/aux/item_pool.hpp>
#include<vector>
#include<memory>
#include<mutex>
#include<iow/memory.hpp>

namespace iow{ namespace io{

struct data_pool_options
{
  size_t poolsize = 1024;
  size_t minbuf = 128;
  size_t maxbuf = 4096;
};

  
template<typename DataType>
class data_pool
{
public:
  typedef DataType data_type;
  typedef std::unique_ptr<data_type>  data_ptr;
  typedef std::mutex mutex_type;
  typedef data_pool_options options_type;
  
  data_pool() noexcept
    : _poolsize(1024)
    , _minbuf(128)
    , _maxbuf(4096)
  {
    _pool.reserve(_poolsize);
  }
  
  template<typename O>
  void set_options(O&& opt) noexcept
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _poolsize = opt.poolsize;
    _minbuf = opt.minbuf;
    _maxbuf = opt.maxbuf;
    if ( _minbuf > _maxbuf )
      _minbuf = _maxbuf;
  }

  template<typename O>
  void get_options(O& opt) const noexcept
  {
    std::lock_guard<mutex_type> lk(_mutex);
    opt.poolsize = _poolsize;
    opt.minbuf = _minbuf;
    opt.maxbuf = _maxbuf;
  }

  data_ptr create()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return this->create_(_minbuf, _maxbuf);
  }

  data_ptr create(size_t bufsize)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return this->create_(bufsize, _maxbuf);
  }
  
  
  data_ptr create(size_t bufsize, size_t maxbuf )
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return this->create_(bufsize, maxbuf);
  }
  
  void free(data_ptr d) noexcept
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( d==nullptr )
      return;
    
    if ( _pool.size() >= _poolsize)
      return;
    
    if ( d->capacity() < _minbuf || d->capacity() > _maxbuf)
      return;
    
    _pool.push_back( std::move(d));
  }
  
  size_t count() const noexcept
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return _pool.size();
  }
  
  size_t capacity() const noexcept
  {
    std::lock_guard<mutex_type> lk(_mutex);
    size_t result = 0;
    for (auto& buf : _pool)
    {
      result += buf.capacity();
    }
    return result;
  }
  
private:
  
  data_ptr create_(size_t bufsize, size_t maxbuf )
  {
    if (_pool.empty() )
      return std::make_unique< data_type >(bufsize);
    
    auto& buf = _pool.back();
    if ( buf->capacity() < bufsize || buf->capacity() > maxbuf )
      return std::make_unique< data_type >(bufsize);
    
    auto result = std::move(buf);
    _pool.pop_back();
    return std::move(result);
  }

  
  typedef std::vector<data_ptr> pool_type;
  pool_type _pool;
  size_t _poolsize;
  size_t _minbuf;
  size_t _maxbuf;
  mutable std::mutex _mutex;
};

  /*
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
*/

}}
