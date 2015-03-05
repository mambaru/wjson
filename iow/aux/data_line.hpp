#pragma once

#include <iow/aux/memory.hpp>
#include <queue>
#include <memory>
#include <stdexcept>
#include <boost/concept_check.hpp>

namespace iow{ 

#warning Возвращать итераторы, чтобы не перемещать при подтверждении, а при полученни перемещать в wait object
  
class confirm_error
  : public std::logic_error
{
public:
  confirm_error()
    : std::logic_error( "iow::data_line: confirm_error" )
  {
  }
};

struct data_line_options
{
  size_t bufsize = 8*1024;
  size_t maxbuf  = 8*1024; 
  size_t minbuf  = 0; 
  // Пул буфферов, для размещения 
  size_t poolsize = 0;
  // Наверное убрать отсюда
  size_t wrnsize = 1024*1024;
  size_t maxsize = 1024*1024*1024;

  // TODO: дурацкие название
  bool except_first = true; // Если maxbuff или minbuff != 0 и bufsize!=0
  bool except_confirm = true;
};


template<typename DataType>
class data_line
{
public:
  typedef data_line_options options_type;
  typedef DataType data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef typename data_type::iterator iterator;
  
  data_line() noexcept
    : _options()
    , _size(0)
  {
  }
  
  void set_options(const options_type& options) noexcept
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

  const options_type& options() const noexcept
  {
    return _options;
  }

  data_ptr next() 
  {
    return this->next(nullptr);
  }
  
  data_ptr next(data_ptr d) noexcept
  {
    if ( _line.empty() )
      return this->next_first_(std::move(d));
    else
      return this->next_rest_(std::move(d));
  }
  
  void confirm() noexcept
  {
    if ( !this->check_confirm_() )
      return;
    
    _line.pop_front();
  }

  // Если данные отправленны все 
  data_ptr confirm_and_next() noexcept
  {
    this->confirm();
    if ( _line.empty() )
      return nullptr;
    _size -= _line.front()->size();
    return std::move( _line.front() );
  }

  data_ptr confirm_and_next(data_ptr d, size_t size_confirmed) noexcept
  {
    if ( !this->check_confirm_() )
    {
      if ( d!=nullptr )
      {
        this->push_back_(std::move(d), size_confirmed);
      }
      return this->next();
    }

    if ( d==nullptr || d->size() == size_confirmed )
    {
      return this->confirm_and_next();
    }

    if (d->size() < size_confirmed )
    {
      size_confirmed = d->size();
    }
    
#warning Разбить хвост, если большой и вставить в начало
    
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
         _line.front() = nullptr;
      }
    }
    
    return std::move(d);
  }

  void rollback(data_ptr d) noexcept
  {
    if ( _line.empty() )
      return; // TODO exception
    if ( _line.front()!=nullptr )
      return; // TODO exception
    _line.front() = std::move(d);
    if (d!=nullptr)
      _size += d->size();
  }

  void push_back(data_ptr d) noexcept
  {
    if ( d == nullptr )
      return;
    size_t offset = this->merge_(d->begin(), d->end());
    this->push_back_( std::move(d), offset );
  }

  bool unconfirmed() const noexcept
  {
    return !_line.empty() && _line.front()==nullptr;
  }

  size_t size() const noexcept
  {
    return _size;
  }

  size_t count() const noexcept
  {
    return _line.size() - this->unconfirmed();
  }

  bool is_full() const noexcept
  {
    return _options.maxsize!=0 && _size > _options.maxsize;
  }

  bool is_wrn() const noexcept
  {
    return _options.wrnsize!=0 && _size > _options.wrnsize;
  }

  void clear() noexcept
  {
    _line.clear();
    _size = 0;
  }

private:

  data_ptr next_first_(data_ptr d) noexcept
  {
    if ( d == nullptr )
      return nullptr;

    _line.push_back(nullptr);

    if ( _options.except_first || d->size() <=  _options.maxbuf)
    {
      return std::move(d);
    }

    this->push_back_( d->begin() + _options.bufsize, d->end() );
    d->resize(_options.bufsize);
    return std::move(d);
  }

  data_ptr next_rest_(data_ptr d) noexcept
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
    _size -= d->size();
    return std::move(d);
  }


  bool check_confirm_() noexcept
  {
    return !_line.empty() && _line.front()==nullptr;
  }

  void push_back_(iterator beg, iterator end) noexcept
  {
    if ( beg == end )
      return;

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
  
  void push_back_(data_ptr d, size_t offset) noexcept
  {
    if (d->size()==offset)
      return;
    
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
  size_t merge_( iterator beg, iterator end ) noexcept
  {
    if ( beg == end )
      return 0;
    
    size_t size=0;
    // Объединить с последним элементом?
    bool merge = !_line.empty() && _line.back()!=nullptr && _line.back()->size() < _options.minbuf;
    
    if ( merge )
    {
      auto& d = _line.back();
      size_t backsize = d->size();
      size_t cursize = std::distance(beg, end);
      
      // Сколько скопировать в хвост
      size = _options.bufsize - backsize;
      
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
  options_type _options;
  size_t _size;
  std::deque<data_ptr> _line;
};

}
