#pragma once

#include <iow/aux/memory.hpp>
#include <vector>
#include <memory>

namespace iow{ 
  
template<typename DataType>
class data_line
{
public:
  
  typedef DataType data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
  data_line()
    : _bufsize(0)
    , _wrnsize(0)
    , _maxsize(0)
    , _size(0)
  {}
  
  void init(size_t bufsize, size_t wrnsize,  size_t maxsize) 
  {
    _bufsize = bufsize;
    _wrnsize = wrnsize;
    _maxsize = maxsize;
  }
  
  data_ptr create(data_ptr d)
  {
    if ( d == nullptr )
    {
      // Взять следующую порцию (если есть)
      
      if ( !_line.empty() && _line.front()==nullptr )
      {
        // Это подверждение отправки
        _line.pop_front();
      }

      if ( !_line.empty() )
      {
        // Если есть еще данные 
        d = std::move(_line.front() );
        _size -= d->size();  
        return std::move(d);
      }
    }
    else
    {
      // При пустой очереди, сразу возвращаем
      if ( _line.empty() )
      {
        // Признак того, что ожидаем подверждение
        _line.push_back(nullptr);
        return std::move(d);
      }

      // Игногируем при переполнении 
      if ( this->is_full() )
        return nullptr;

      // Очередь не пуста, ставим в конец
      _size += d->size();
      if ( _line.back()!=nullptr && (_line.back()->size()) + d->size() <= _bufsize )
      {
        _line.back()->reserve(_bufsize);
        std::copy(d->begin(), d->end(), std::inserter( _line.back()->end(), *(_line.back())));
      }
      else
      {
        _line.push_back( std::move(d) );
      }
      
    }
    return nullptr;
  }
  
  /*
  void confirm()
  {
    if ( !_line.empty() && _line.front()==nullptr )
    {
      _line.pop_front();
    }
  }*/

  size_t size() const 
  {
    return _size;
  }
  
  bool is_full() const
  {
    return _maxsize!=0 && _size > _maxsize;
  }
  
  bool is_wrn() const
  {
    return _wrnsize!=0 && _size > _wrnsize;
  }
  
private:
  size_t _bufsize;
  size_t _wrnsize;
  size_t _maxsize;
  size_t _size;
  std::queue<data_ptr> _line;
};

}
