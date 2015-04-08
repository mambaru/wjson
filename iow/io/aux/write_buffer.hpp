#pragma once

#include <iow/memory.hpp>
#include <queue>
#include <memory>


namespace iow{ namespace io{


template<typename DataType>
struct write_buffer_options
{
  typedef std::unique_ptr<DataType> data_ptr;
  typedef std::function< data_ptr(size_t, size_t) > create_fun;
  typedef std::function< void(data_ptr) > free_fun;
  std::string sep;

  size_t bufsize = 8*1024;
  size_t maxbuf  = 8*1024; 
  size_t minbuf  = 0; 
  // Пул буфферов, для размещения 
  //size_t poolsize = 0;
  // Наверное убрать отсюда
  //size_t wrnsize = 1024*1024;
  //size_t maxsize = 1024*1024*1024;

  bool first_as_is = true; // Если maxbuff или minbuff != 0 и bufsize!=0
  
  create_fun create;
  free_fun free;

  /*
  
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
  */
};


template<typename DataType>
class write_buffer
{
public:
  typedef write_buffer_options<DataType> options_type;
  typedef std::shared_ptr<options_type> options_ptr;
  typedef DataType data_type;
  typedef typename data_type::value_type value_type;
  typedef const value_type* value_ptr;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::unique_ptr<value_type[]> sep_ptr;
  typedef std::pair<value_ptr, size_t> data_pair;
  typedef std::vector<data_ptr> data_list;
  //typedef std::unique_ptr<deque_list> deque_ptr;
  
  typedef std::function<data_ptr(size_t, size_t)> create_fun;
  typedef std::function<void(data_ptr)> free_fun;

  
public:
  
  write_buffer() noexcept
    : _sep(nullptr)
    , _sep_size(0)
    , _bufsize(8*1024)
    , _maxbuf(8*1024)
    , _minbuf(0) 
    , _first_as_is(true)
    , _create(nullptr)
    , _free(nullptr)
    , _size(0)
    , _offset(0)
    , _wait(0)
    
  {
    _list.reserve(2);
  }
  
  /*
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
  */

  void clear()
  {
    _size = 0;
    _offset = 0;
    //_cur = nullptr;
    //_list = nullptr;
    _list.clear();
    _wait = 0;
  }

  
  template<typename O>
  void set_options(const O& opt) noexcept
  {
    if ( opt.sep.empty() )
    {
      _sep = nullptr;
      _sep_size=0;
    }
    else
    {
      _sep_size = opt.sep.size();
      _sep=sep_ptr(new value_type[_sep_size]);
      std::copy(opt.sep.begin(), opt.sep.end(), _sep.get() );
    }

    _bufsize = opt.bufsize;
    _maxbuf = opt.maxbuf;
    _minbuf = opt.minbuf;
    _first_as_is = opt.first_as_is;

    _create = opt.create;
    _free = opt.free;

    if ( _bufsize == 0 )
    {
      _bufsize = 4096;
    }
    
    if ( _minbuf==0 )
    {
      _minbuf=128;
    }

    if ( _minbuf > _bufsize )
    {
      _minbuf = _bufsize;
    }

    if ( _maxbuf < _bufsize )
    {
      _maxbuf = _bufsize;
    }

    _list.reserve(2);
    /*
    if ( _maxsize == 0 )
    {
      _maxsize = 4096*1024;
    }
    */
  }

  template<typename O>
  void get_options(O& opt) const noexcept
  {
    if ( _sep_size!=0 )
    {
      opt.sep.assign(_sep.get(), _sep.get() + _sep_size);
    }
    else
    {
      opt.sep.clear();
    }
    
    opt.bufsize = _bufsize;
    // opt.maxsize = _maxsize;
    opt.maxbuf  = _maxbuf;
    opt.minbuf  = _minbuf;
    opt.first_as_is = _first_as_is;
    opt.create  = _create;
    opt.free    = _free;
  }
  
  /*
  void set_options(options_ptr options) noexcept
  {
    _options = options;
  }

  options_ptr get_options() const noexcept
  {
    return _options;
  }
  */

  size_t size() const noexcept
  {
    return _size;
  }

  size_t count() const noexcept
  {
    //return ( _list!=nullptr ? _list->size() : 0) + static_cast<size_t>( _cur!=nullptr );
    return _list.size();
  }
  
  size_t offset() const noexcept
  {
    return _offset;
  }

  size_t capacity() const noexcept
  {
    size_t result = 0;
    for ( auto& d : _list )
      result += _list->capacity();

       /*
    size_t result = _cur!=nullptr ? _cur->capacity() : 0;
    if ( _list!=nullptr )
    {
      for ( auto& d : _list )
        result += _list->capacity();
    }
    */
    return result;
  }

  bool ready() const
  {
    //return _wait == 0 && _cur!=nullptr;
    return _wait == 0 && !_list.empty();
  }

  bool waiting() const
  {
    return _wait != 0;
  }
  
  /*
   * TODO: проверяет 
  bool check(const data_type& d) const
  {
    return _options==nullptr || ( _size + _offset + d.size() <= _options->maxsize );
  }
  */
  
  void addsep_( data_type& d, bool reserve )
  {
    if (_sep_size==0)
      return;
    
    if (reserve)
      d.reserve( d.size() + _sep_size );
    
    std::copy( _sep.get(), _sep.get() + _sep_size, std::inserter(d, d.begin()) );
    
  }

  void attach(data_ptr d)
  {
    if ( d==nullptr || ( d->empty() && _sep_size==0 ) )
      return;
    
    
    /*
    if ( !check(*d) )
    {
      return std::move(d);
    }
    */
    
    

    _size += d->size();

    if ( _list.empty() )
    {
      addsep_(*d, true);
      _list.push_back( std::move(d) );
    }
    else
    {
      /*
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
      */
      data_ptr& last = _list.back();
      size_t sumsize = last->size() + d->size() + _sep_size;
      if ( last->size() < _minbuf && sumsize < _maxbuf )
      {
        last->reserve(sumsize);
        std::copy( d->begin(), d->end(), std::inserter(*last, last->end() ) );
        addsep_(*last, false);
        free_(std::move(d));
      }
      else
      {
        addsep_(*d, true);
        _list.push_back( std::move(d) );
      }
    }
    
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

  bool confirm(data_pair p)
  {
    bool result = false;

    if ( _wait == 0 || _size < p.second ) 
      return result;

    _wait = 0;
    _offset += p.second;
    _size -= p.second;
    
    if ( _offset == _list.front()->size())
    {
      _offset = 0;
      free_( std::move(_list.front()) );
      //_list.pop_front();
      _list.erase( _list.begin() );
    }
    
    // TODO: убрал копирование для minbuf
    return true;
  }

  void rollback()
  {
    if ( _wait != 0 )
      return;
    _wait = 0;
  }
  
private:
  
  data_ptr create_(size_t size, size_t maxbuf) const
  {
    if ( _create!=nullptr )
      return _create(size, maxbuf);
    return std::make_unique<data_type>(size);
  }

  
  data_ptr create_(size_t size) const
  {
    return this->create_(size, _bufsize);
  }

  data_ptr create_() const
  {
    return this->create_(_bufsize);
  }
  
  void free_(data_ptr d) const
  {
    if ( _free != nullptr)
      _free( std::move(d) );
  }

  
  value_ptr cur_ptr_() const
  {
    return &(_list.front()->operator[](0)) + _offset;
    //return &(_cur->operator[](0)) + _offset;
  }

  size_t cur_size_() const
  {
    size_t size = _list.front()->size() - _offset;
    bool first_as_is = _first_as_is && _offset==0;
    if ( !first_as_is && size > _maxbuf )
    {
      size = _maxbuf;
    }
    return size;
    /*
    size_t size = _cur->size() - _offset;
    
    if ( _options!=nullptr )
    {
      bool first_as_is = _options->first_as_is && _offset==0 && ( _list == nullptr || _list->empty() );
      if ( !first_as_is && size > _options->maxbuf )
        size = _options->bufsize;
    }
    return size;
    */
  }

private:
  
  // options
  sep_ptr _sep;
  size_t _sep_size;
  size_t _bufsize;
  size_t _maxbuf;
  size_t _minbuf;
  bool _first_as_is;
  create_fun _create;
  free_fun _free;

  // -------------------
  size_t _size;
  size_t _offset;
  size_t _wait; // в байтах
  /* options_ptr _options;
     data_ptr    _cur;
  */
  //deque_ptr   _list;
  data_list _list;
};


}}
