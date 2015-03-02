#pragma once

#include <iow/aux/memory.hpp>
#include <queue>
#include <memory>
#include <boost/concept_check.hpp>

namespace iow{ 

enum struct data_line_split_mode{
  disable,        // [default] запретить разбиение пакетов
  enable,         // Разбивать все пакеты, которые большe buffsize
  except_first,   // Исключить первый, но разбить остаток при подтверждении
  except_confirm  // Исключить первый и не разбивать остаток при подтверждении
};

enum struct data_line_merge_mode{
  // Объединение происходит только если линия не пуста
  disable,        // [default] запретить объединение пакетов
  enable,         // Объеденять со следующим, если суммарный размер не превышает buffsize
  except_confirm  // Не объединять со следующим при подтверждении
};

enum struct data_line_mode{

};

struct data_line_options
{
  typedef data_line_split_mode split_mode;
  typedef data_line_merge_mode merge_mode;
  typedef data_line_mode mode;
  ;
  size_t bufsize;           // 8KB, 0 - without buffer splitting
  size_t wrnsize;           // 1MB
  size_t maxsize;           // 1GB
  //split_mode split;
  //merge_mode merge;
  //bool strong_mode; //?? default=false, при false допустимо двухкратное превышение буфера 
  
  size_t maxbuff; // Если меньше bufsize, то == bufsize
  size_t minbuff; // Если больше bufsize, то == bufsize
  // TODO: дурацкие название
  bool except_first; // Если maxbuff или minbuff != 0 и bufsize!=0
  bool except_confirm;
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


  data_ptr confirm(data_ptr d, size_t size_confirmed)
  {
    if ( _line.empty() )
      return nullptr; // TODO exception
    if ( _line.front()!=nullptr )
      return nullptr; // TODO exception
    // TODO: split or merge if 
    return nullptr;
  }


  void rollback(data_ptr d)
  {
    if ( _line.empty() )
      return; // TODO exception
    if ( _line.front()!=nullptr )
      return; // TODO exception
    // TODO: split or merge if 
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
