#pragma once

#include <iow/memory.hpp>
#include <queue>
#include <memory>


namespace iow{ namespace io{


struct write_buffer_options
{
  size_t bufsize = 8*1024;
  size_t maxbuf  = 8*1024; 
  size_t minbuf  = 0; 
  // Пул буфферов, для размещения 
  //size_t poolsize = 0;
  // Наверное убрать отсюда
  //size_t wrnsize = 1024*1024;
  size_t maxsize = 1024*1024*1024;

  bool first_as_is = true; // Если maxbuff или minbuff != 0 и bufsize!=0
  
  
  void fix()
  {
    if ( bufsize == 0 )
    {
      bufsize = 1024 * 8;
    }
    
    if ( minbuf > bufsize )
    {
      minbuf = bufsize;
    }
    
    if ( maxbuf < bufsize )
    {
      maxbuf = bufsize;
    }
    
    if ( maxsize == 0 )
    {
      maxsize = 1024*1024*1024;
    }
  }
};


template<typename DataType>
class write_buffer
{
public:
  typedef write_buffer_options options_type;
  typedef std::shared_ptr<options_type> options_ptr;
  typedef DataType data_type;
  typedef typename data_type::value_type value_type;
  typedef const value_type* value_ptr;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::pair<value_ptr, size_t> data_pair;
  typedef std::deque<data_ptr> deque_list;
  typedef std::unique_ptr<deque_list> deque_ptr;
  
public:
  
  write_buffer() noexcept
    : _size(0)
    , _offset(0)
    , _wait(0)
    , _options(nullptr)
  {
  }
  
  explicit write_buffer(options_ptr options) noexcept
    : _size(0)
    , _offset(0)
    , _wait(0)
    , _options(options )
  {
  }

  write_buffer(const write_buffer& other) noexcept
    : _size(0)
    , _offset(0)
    , _wait(0)
    , _options(other._options)
  {
  }

  write_buffer(write_buffer&& other) noexcept
    : _size( other._size)
    , _offset( other._offset)
    , _cur( std::move(other._cur))
    , _list( std::move(other._list))
    , _wait( other._wait)
    , _options( other._options)
  {
  }

  void clear()
  {
    _size = 0;
    _offset = 0;
    _cur = nullptr;
    _list = nullptr;
    _wait = 0;
  }

  void set_options(options_ptr options) noexcept
  {
    _options = options;
  }

  options_ptr get_options() const noexcept
  {
    return _options;
  }

  size_t size() const noexcept
  {
    return _size;
  }

  size_t count() const noexcept
  {
    return ( _list!=nullptr ? _list->size() : 0) + static_cast<size_t>( _cur!=nullptr );
  }
  
  size_t offset() const noexcept
  {
    return _offset;
  }

  size_t capacity() const noexcept
  {
    size_t result = _cur!=nullptr ? _cur->capacity() : 0;
    if ( _list!=nullptr )
    {
      for ( auto& d : _list )
        result += _list->capacity();
    }
    return result;
  }

  bool ready() const
  {
    return _wait == 0 && _cur!=nullptr;
  }

  bool waiting() const
  {
    return _wait != 0;
  }
  
  bool check(const data_type& d) const
  {
      return _options==nullptr || ( _size + _offset + d.size() <= _options->maxsize );
  }

  data_ptr attach(data_ptr d)
  {
    if ( d==nullptr || d->empty() )
      return nullptr;
    
    if ( !check(*d) )
    {
      return std::move(d);
    }

    _size += d->size();

    if ( _cur == nullptr )
    {
      _cur = std::move(d);
    }
    else
    {
      if ( _options!=nullptr )
      {
        data_ptr& last = _list!=nullptr && !_list->empty()
          ? _list->back()
          : _cur;
        size_t sumsize = last->size() + d->size();
        if ( sumsize < _options->minbuf )
        {
          last->reserve(sumsize);
          std::copy( d->begin(), d->end(), std::inserter(*last, last->end() ) );
          return std::move(d);
        }
      }

      if ( _list==nullptr )
      {
        _list = std::make_unique<deque_list>();
      }
      _list->push_back( std::move(d) );
    }
    return nullptr;
  }

  data_pair next()
  {
    if ( !this->ready() )
      return data_pair();

    auto size = this->cur_size_();
    auto ptr  = this->cur_ptr_();
    _wait = size;

    return data_pair( ptr, size );
  }

  data_ptr confirm(data_pair p)
  {
    data_ptr result = nullptr;

    if ( _wait == 0 || _size < p.second ) 
      return nullptr;

    _wait = 0;
    _offset += p.second;
    _size -= p.second;
    if ( _offset == _cur->size() )
    {
      result = std::move(_cur);
      _offset = 0;

      if (_list!=nullptr && !_list->empty() )
      {
        _cur = std::move( _list->front() );
        _list->pop_front();
      }
    }
    else if ( _options!=nullptr && _cur->size() < _options->minbuf )
    {
      if ( _list!=nullptr && !_list->empty() )
      {
        auto& front = _list->front();
        size_t sumsize = _cur->size() + front->size();
        if ( sumsize < _options->maxbuf )
        {
          front->reserve(sumsize);
          std::copy( front->begin(), front->end(), std::inserter(*_cur, _cur->end()) );
          _list->pop_front();
        }
      }
    }
    return std::move(result);
  }

  void rollback()
  {
    if ( _wait != 0 )
      return;
    _wait = 0;
  }
  
private:
  
  value_ptr cur_ptr_() const
  {
    return &(_cur->operator[](0)) + _offset;
  }

  size_t cur_size_() const
  {
    size_t size = _cur->size() - _offset;
    
    if ( _options!=nullptr )
    {
      bool first_as_is = _options->first_as_is && _offset==0 && ( _list == nullptr || _list->empty() );
      if ( !first_as_is && size > _options->maxbuf )
        size = _options->bufsize;
    }
    return size;
  }

private:
  size_t _size;
  size_t _offset;
  size_t _wait; // в байтах
  options_ptr _options;
  data_ptr    _cur;
  deque_ptr   _list;
};


}}
