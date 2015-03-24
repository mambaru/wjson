#pragma once

#include <iow/memory.hpp>
#include <queue>
#include <memory>
#include <stdexcept>
#include <boost/concept_check.hpp>
//#include <boost/concept_check.hpp>

namespace iow{ namespace io{

//#warning Возвращать итераторы, чтобы не перемещать при подтверждении, а при полученни перемещать в wait object. Или сразу boost::asio::buffer?
  
class confirm_error
  : public std::logic_error
{
public:
  confirm_error()
    : std::logic_error( "iow::data_line: confirm_error" )
  {
  }
};

struct data_stream_options
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
  }
};


template<typename DataType>
class data_stream
{
public:
  typedef data_stream_options options_type;
  typedef std::shared_ptr<options_type> options_ptr;
  typedef DataType data_type;
  typedef typename data_type::value_type value_type;
  typedef const value_type* value_ptr;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::pair<value_ptr, size_t> data_pair;
  typedef std::deque<data_ptr> deque_list;
  typedef std::unique_ptr<deque_list> deque_ptr;
  
public:
  
  data_stream() noexcept
    : _options(nullptr)
    , _size(0)
    , _offset(0)
    , _wait(0)
    , _error(false)
  {
    
  }
  
  explicit data_stream(options_ptr options) noexcept
    : _options(options )
    , _size(0)
    , _offset(0)
    , _wait(0)
    , _error(false)
  {
  }

  data_stream(const data_stream& other) noexcept
    : _options(other._options)
    , _size(0)
    , _offset(0)
    , _wait(0)
    , _error(false)
  {
  }

  data_stream(data_stream&& other) noexcept
    : _options( other._options)
    , _size( other._size)
    , _offset( other._offset)
    , _cur( std::move(other._cur))
    , _list( std::move(other._list))
    , _wait( other._wait)
    , _error( other._error)
  {
  }

  void clear()
  {
    _size = 0;
    _offset = 0;
    _cur = nullptr;
    _list = nullptr;
    _wait = 0;
    _error = false;
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
    return (_list!=nullptr ? _list->size() : 0) + static_cast<size_t>( _cur!=nullptr );
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


  void entry(data_ptr d)
  {
    if ( d==nullptr || d->empty() )
      return;

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
          return;
        }
      }

      if ( _list==nullptr )
      {
        _list = std::make_unique<deque_list>();
      }
      _list->push_back( std::move(d) );
    }
  }


  data_pair next()
  {
    if ( !this->ready() )
      return data_pair();

    auto size = this->cur_size_();
    auto ptr  = this->cur_ptr_();
    _wait = size;
    _offset += size;
    if ( _offset > _cur->size() )
      throw;
    return data_pair( ptr, size );
  }

  data_ptr confirm(data_pair p)
  {
    data_ptr result = nullptr;

    if ( this->ready() ) 
      throw; //!!!!

    _wait = 0;
    //_offset += p.second;
    _size -= p.second;
    if ( _offset == _cur->size() )
    {
      result = std::move(_cur);
      _offset = 0;

      if (_list!=nullptr && !_list->empty() )
      {
        _cur = std::move( _list->front() );
        _list->pop_front();
        // TODO: fix shink to fit if maxcount
      }
    }
    else if ( _options!=nullptr && _cur->size() < _options->minbuf )
    {
      // TODO: перенести из _list в хвост _cur, если _cur мал
      throw;
    }
    return std::move(result);
  }

  void rollback(data_pair p)
  {
    
  }
  
private:
  
  value_ptr cur_ptr_() const
  {
    return &(_cur->operator[](0)) + _offset;
  }

  size_t cur_size_() const
  {
    size_t size = _cur->size() - _offset;
    if ( _options!=nullptr &&  size > _options->maxbuf )
    {
      size = _options->bufsize;
    }
    return size;
  }

private:
  
  options_ptr _options;
  size_t _size;
  size_t _offset;
  data_ptr _cur;
  deque_ptr _list;
  size_t _wait; // в байтах
  bool _error; // состояние ошибки
};




template<typename DataType>
class data_stream2
{
public:
  typedef data_stream_options options_type;
  typedef DataType data_type;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef typename data_type::iterator iterator;
  
  data_stream2() noexcept
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
    
// #warning Разбить хвост, если большой и вставить в начало
    
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

}}
