#pragma once

#include <iow/aux/memory.hpp>
#include <queue>
#include <memory>
#include <boost/concept_check.hpp>

namespace iow{ 
 
struct data_line_options
{
  /*
  size_t bufsize = 8192;        // 8KB, 0 - without buffer splitting
  size_t wrnsize = 0;           // 1MB
  size_t maxsize = 0;           // 1GB
  */
  size_t bufsize;        // 8KB, 0 - without buffer splitting
  size_t wrnsize;           // 1MB
  size_t maxsize;           // 1GB
};

template<typename DataType>
class data_line
{
public:
  typedef data_line_options options_type;
  typedef DataType data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
  data_line()
    : _options({8192,0,0})
    , _size(0)
  {}
  
  void set_options(const options_type& options) 
  {
    _options = options;
  }
  
  const options_type& options() const
  {
    return _options;
  }
  
  /**
   * @param d - данные или nullptr
   * @result следующая порция данных
   * 
   */
  data_ptr create(data_ptr d)
  {
    if ( d == nullptr )
    {
      // Взять следующую порцию (если есть)
      
      if ( !_line.empty() && _line.front()==nullptr )
      {
        // Это подверждение отправки
        _line.pop();
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
        _line.push(nullptr);
        return std::move(d);
      }

      // Игногируем при переполнении 
      if ( this->is_full() )
        return nullptr;

      // Очередь не пуста, ставим в конец
      _size += d->size();
      if ( _line.back()!=nullptr && (_line.back()->size()) + d->size() <= _options.bufsize )
      {
        _line.back()->reserve(_options.bufsize);
        std::copy(d->begin(), d->end(), std::inserter( *(_line.back()), _line.back()->end()));
      }
      else
      {
        _line.push( std::move(d) );
      }
      
    }
    return nullptr;
  }
  
  void rollback(data_ptr d)
  {
    _line.front() = std::move(d);
  }
  
  size_t size() const 
  {
    return _size;
  }
  
  bool is_full() const
  {
    return _options.maxsize!=0 && _size > _options.maxsize;
  }
  
  bool is_wrn() const
  {
    return _options.wrnsize!=0 && _size > _options.wrnsize;
  }
  
  void clear()
  {
    while ( !_line.empty() )
    {
      _line.pop();
    }
    _size = 0;
  }
  
private:
  options_type _options;
  size_t _size;
  std::queue<data_ptr> _line;
};

}
