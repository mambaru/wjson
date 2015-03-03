#pragma once

#include <iow/aux/memory.hpp>
#include <list>
#include <memory>
#include <stdexcept>
#include <boost/concept_check.hpp>

namespace iow{ 

  /*
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
*/


template<typename N>
class data_line_excepttion
  : public std::logic_error
{
public:
  data_line_excepttion()
    : std::logic_error( N()() )
  {
  }
};

struct data_line_options
{
  size_t bufsize /*= 8*1024*/;
  size_t maxbuf  /*= 8*1024*/; 
  size_t minbuf  /*= 128*/; 
  size_t wrnsize /*= 1024*1024*/;
  size_t maxsize /*= 1024*1024*1024*/;

  // TODO: дурацкие название
  bool except_first /*= true*/; // Если maxbuff или minbuff != 0 и bufsize!=0
  bool except_confirm /*= true*/;
};

template<typename DataType>
class data_line
{
public:
  typedef data_line_options options_type;
  typedef DataType data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef typename data_type::iterator iterator;
  
  data_line()
    : _options()
    , _size(0)
  {
  }
  
  void set_options(const options_type& options) 
  {
    _options = options;
    
    if ( _options.bufsize == 0 )
    {
      _options.bufsize = 1024 * 8;
    }
    
    if ( _options.minbuf > _options.bufsize )
    {
      _options.minbuf = _options.bufsize;
    }
    
    if ( _options.maxbuf < _options.bufsize )
    {
      _options.maxbuf = _options.bufsize;
    }
  }
  
  const options_type& options() const
  {
    return _options;
  }

  data_ptr next(data_ptr d)
  {
    if ( _line.empty() )
      return this->next_first_(std::move(d));
    else
      return this->next_rest_(std::move(d));
  }

  // Если данные отправленны все 
  data_ptr confirm_next()
  {
    this->check_confirm_();
    _line.pop_front();
    if ( _line.empty() )
      return nullptr;
    _size -= _line.front()->size();
    return std::move( _line.front() );
  }
  
  data_ptr confirm_next(data_ptr d, size_t size_confirmed)
  {
    if ( d->size() == size_confirmed )
      return this->confirm_next();
    
    if (d->size() < size_confirmed )
      throw; // TODO exception
      
    size_t tailsize = d->size() - size_confirmed;
    std::copy( d->begin() + size_confirmed, d->end(), d->begin() ); 
    d->resize(tailsize);
    
    size_t cursize = d->size();
    if ( !_options.except_confirm && cursize < _options.minbuf && _line.size() > 1 )
    {
      auto itr = _line.begin() + 1;
      size_t nextsize = (*itr)->size();
      if ( cursize + nextsize < _options.maxbuf )
      {
        std::copy( (*itr)->begin(), (*itr)->end(), std::inserter( *d, d->end()));
        _size -= (*itr)->size();
        _line.pop_front();
        _line.back() = nullptr;
      }
    }
  }

  void rollback(data_ptr d)
  {
    if ( _line.empty() )
      return; // TODO exception
    if ( _line.front()!=nullptr )
      return; // TODO exception
    _line.front() = std::move(d);
    if (d!=nullptr)
      _size += d->size();
  }

  void push_back(data_ptr d)
  {
    if ( d == nullptr )
      return;
    size_t offset = this->merge_(d->begin(), d->end());
    this->push_back_( std::move(d), offset );
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
    _line.clear();
    _size = 0;
  }

private:
  
  data_ptr next_first_(data_ptr d)
  {
    if ( d == nullptr )
      return nullptr;

    if ( _options.except_first )
      return std::move(d);
    
    if ( d->size() <  _options.maxbuf )
      return std::move(d);
    
    this->push_back_( d->begin() + _options.bufsize, d->end() );
    d->resize(_options.bufsize);
    return std::move(d);
  }

  data_ptr next_rest_(data_ptr d)
  {
    if ( d!=nullptr )
    {
      this->push_back( std::move(d) );
    }
    
    if ( _line.front() == nullptr )
    {
      return nullptr;
    }
    
    d = std::move( _line.front() );
    _line.pop_front();
    _size -= d->size();
    return std::move(d);
  }

  
  void check_confirm_()
  {
    if ( _line.empty() )
      throw ; // TODO exception
    if ( _line.front()!=nullptr )
      throw ; // TODO exception
  }
  
  void push_back_(iterator beg, iterator end)
  {
    auto cur = beg; 
    while (beg!=end)
    {
      cur = std::distance(beg,end) < static_cast<std::ptrdiff_t>(_options.maxbuf)
            ? end
            : beg + _options.bufsize;
        
      data_ptr d = std::make_unique<data_type>(beg, cur);
      _size += d->size();
      _line.push_back(std::move(d));
      beg = cur;
    }
  }
  
  void push_back_(data_ptr d, size_t offset)
  {
    _line.push_back( std::move(d) );
    auto& dd = _line.back();
    bool more = std::distance( dd->begin() + offset, dd->end() ) > static_cast<std::ptrdiff_t>(_options.maxbuf);
    if ( more )
    {
      size_t tailpos = offset + _options.bufsize;
      this->push_back_(dd->begin() + tailpos, dd->end() );
      std::copy( dd->begin() + offset, dd->begin() + tailpos, dd->begin() );
      dd->resize(_options.bufsize);
    }
    else
    {
      std::copy( dd->begin() + offset, dd->end(), dd->begin() );
      dd->resize(dd->size() - offset );
    }
    _size += dd->size();
  }
  
  // Объеденить с хвостом (если имеет смысл)
  size_t merge_( iterator beg, iterator end )
  {
    size_t size=0;
    // Объединить с последним элементом?
    bool merge = !_line.empty() && _line.back()!=nullptr && _line.back()->size() < _options.minbuf;
    
    if ( merge )
    {
      auto& d = _line.back();
      size_t backsize = d->size();
      size_t cursize = std::distance(beg, end);
      
      // Сколько скопировать в хвост
      size_t size = _options.bufsize - backsize;
      
      bool fulmerge = ( cursize <= size )
                        || ( cursize - size < _options.minbuf 
                             && backsize + cursize < _options.maxbuf );
                        
      if ( fulmerge )
      {
        size = cursize;
      }
      d->reserve(d->size() + size);
      std::copy(
        beg, 
        beg + size,
        std::inserter( *d, d->end() )
      );
      _size += size;
    }
    return size;
  }

private:
  /**
   * @param d - данные или nullptr
   * @result следующая порция данных
   * 
   */
  data_ptr create_old(data_ptr d)
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

  
private:
  options_type _options;
  size_t _size;
  std::list<data_ptr> _line;
};

}
