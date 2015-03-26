#pragma once

#include <iow/memory.hpp>
#include <queue>
#include <memory>

namespace iow{ namespace io{
  
template<typename DataType>
struct read_buffer_options
{
  /*typedef data_pool<DataType> pool_type;
  typedef std::shared_ptr<pool_type> pool_ptr;
  */
  
  // Рекомендованный размер буфера для чтения
  size_t bufsize = 8*1024;
  
  DataType sep;
  // Макс. размер буфера, далее идет расщепление ()
  size_t maxbuf  = 8*1024; 
  // Мин. размер буфера, далее идет объединение 
  size_t minbuf  = 0; 
  // максимальный размер всех буферов
  size_t maxsize = 1024*1024*1024;

};

template<typename DataType>
class read_buffer
{
public:
  typedef read_buffer_options<DataType> options_type;
  typedef std::shared_ptr<options_type> options_ptr;
  typedef DataType data_type;
  typedef typename data_type::value_type value_type;
  typedef value_type* value_ptr;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::pair<value_ptr, size_t> data_pair;
  typedef std::deque<data_ptr> deque_list;
  typedef std::unique_ptr<deque_list> deque_ptr;

public:
  
  read_buffer() noexcept
    : _size(0)
    , _outoff(0)
    , _wait(nullptr)
    , _outbuf(nullptr)
    , _options(nullptr)
  {
  }
  
  explicit read_buffer(options_ptr options) noexcept
    : _size(0)
    , _outoff(0)
    , _wait(nullptr)
    , _outbuf(nullptr)
    , _options(options )
  {
  }

  read_buffer(const read_buffer& other) noexcept
    : _size(0)
    , _outoff(0)
    , _wait(0)
    , _options(other._options)
  {
  }

  read_buffer(read_buffer&& other) noexcept
    : _size( other._size)
    , _outoff( other._offset)
    , _outbuf( std::move(other._outbuf))
    , _outlist( std::move(other._outlist))
    , _wait( other._wait)
    , _options( other._options)
  {
  }

  void clear()
  {
    _size = 0;
    _outoff = 0;
    _outbuf = nullptr;
    _outlist = nullptr;
    _wait = nullptr;
  }

  void set_options(options_ptr options) noexcept
  {
    _options = options;
  }

  options_ptr get_options() const noexcept
  {
    return _options;
  }
  
  data_ptr attach(data_ptr d)
  {
    if ( _inbuf != nullptr )
      return std::move(d);
    
    _inbuf = std::move(d);
    return nullptr;
  }
  
  // Следующий для чтения
  data_pair next()
  {
    if ( _wait != nullptr || _inbuf == nullptr)
      return data_pair();
    _wait = std::move( _inbuf );
    return data_pair( &_wait->front(), _wait->size() );
  }
  
  void confirm(data_pair d)
  {
    if ( _wait == nullptr )
      return;
      
    _wait->resize( d.second );
    
    if ( _outbuf == nullptr )
    {
      _outbuf = std::move(_wait);
    }
    else
    {
      if ( _outlist==nullptr )
      {
        _outlist = std::make_unique<deque_list>();
      }
      _outlist->push_back( std::move(_wait) );
    }
  }
  
  void switch_next_()
  {
    if ( _outlist!=nullptr && !_outlist->empty() )
    {
      _outbuf = std::move(_outlist->front());
      _outlist->pop_front();
    }
  }
  
  // Забрать распарсенный блок
  data_ptr detach()
  {
    if ( _outbuf == nullptr )
      return nullptr;
    
    data_ptr result = nullptr;
    if (_options==0 || _options->sep.empty() )
    {
      result = std::move(_outbuf);
      switch_next_();
    }
    else
    {
      auto p = search_();
      if ( p.first==1 )
      {
        if ( p.second == _outbuf->size() )
        {
          result == std::move(_outbuf);
          if ( _outoff!=0 )
          {
            std::copy( result->begin() + _outoff, result->end(), result->begin() );
            result->resize( result->size() - _outoff);
            _outoff = 0;
          }
          switch_next_();
        }
        else
        {
          result = std::make_unique<data_type>( _outbuf->begin() + _outoff, _outbuf->begin() + p.second);
          _outoff += result->size();
        }
      }
      else
      {
        size_t size = _outbuf->size() - _outoff;
        auto itr = _outlist->begin();
        for (size_t i = 0; i < p.first - 2; ++i, ++itr )
          size += (*itr)->size();
        size += p.second;
        result = std::make_unique<data_type>();
        result->reserve(size);
        std::copy( _outbuf->begin() + _outoff, _outbuf->end(), std::inserter(*result, result->end() ) );
        itr = _outlist->begin();
        for (size_t i = 0; i < p.first - 2; ++i, ++itr )
        {
          std::copy( _outlist->front()->begin(), _outlist->front()->end(), std::inserter(*result, result->end() ) );
          _outlist->pop_front();
        }
        switch_next_();
        // в _outbuf теперь последний
        std::copy( _outbuf->begin(),  _outbuf->begin() + p.second, std::inserter(*result, result->end() ) );
        if ( p.second == _outbuf->size() )
        {
          switch_next_();
          _outoff = 0;
        }
        else
        {
          _outoff = p.second;
        }
      }
    }
      
    return std::move(result);
  }
  
  // first количество буфферов
  // pos   позиция в буфуре, до которой копировать
  std::pair<size_t, size_t>
  search_()
  {
    /*
    auto& last = _outlist!=0 && !_outlist->empty() 
                 ? _outlist->front()
                 : _outbuf;
    ;
    return std::pair<size_t, size_t>();
    */
  }

private:
  
  size_t _size;
  size_t _outpos;
  size_t _outoff; // offset в буфере который был отдан
  data_ptr _wait; // текущий буфер для чтения 
  options_ptr _options;
  data_ptr    _inbuf;
  data_ptr    _outbuf;
  deque_ptr   _outlist;

};

}}
