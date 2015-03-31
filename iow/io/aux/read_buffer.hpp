#pragma once

#include <iow/memory.hpp>
#include <fas/typemanip/empty_type.hpp>
#include <queue>
#include <memory>
#include <algorithm>
#include <boost/concept_check.hpp>

namespace iow{ namespace io{
  
template<typename SepType>
struct read_buffer_options
{
  SepType sep;
};


template<typename DataType, typename SepType>
class read_buffer
{
public:
  typedef DataType data_type;
  typedef typename data_type::value_type value_type;
  typedef value_type* value_ptr;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::unique_ptr<value_type[]> sep_ptr;
  typedef std::pair<value_ptr, size_t> data_pair;
  typedef std::function<data_ptr(size_t)> create_fun;
  typedef std::function<vois(data_ptr)> free_fun;

public:

  read_buffer()
  {}

  template<typename O>
  void set_options(const O& /*opt*/) noexcept
  {
  }

  template<typename O>
  void get_options(O& opt) const noexcept
  {
  }

  size_t count() const
  {
    return _outlist.size();
  }

  /// @return true если необходимо приатачить буфер 
  bool need_buffer() const
  {
    return _inbuf == nullptr;
  }
  
  // Можем приатачить один буфер
  /// @return nullptr если был приаттачен, либо будет вернут обратно, если это сделать невозможно 
  data_ptr attach(data_ptr d)
  {
    if ( !this->need_buffer() )
      return std::move(d);
    
    _inbuf = std::move(d);
    return nullptr;
  }
  
  data_pair next()
  {
    data_pair result(0,0);
    return result;
  }
  
  bool confirm(data_pair d)
  {
    if ( _wait == nullptr )
      return false;

    if ( &(_wait->front()) + _wait_offset - d.first != 0 )
      return false;

    if ( _wait->size() < _wait_offset + d.second )
      return false;

    _wait->resize( _wait_offset + d.second );
    _outlist.push_back( std::move(_wait) );
    return true;
  }
  
  // Забрать распарсенный блок
  data_ptr detach()
  {
    if ( _outlist.empty() )
      return nullptr;
    
    if ( _sep_size == 0 )
    {
      auto result = std::move(_outlist.front());
      _outlist.pop_front();
      return std::move(result);
    }
    
    // TODO:
    return nullptr;
  }

private:
  typedef std::deque<data_ptr> buffer_list;
  //typedef std::unique_ptr<deque_list> deque_ptr;
  //typedef std::pair<size_t, size_t> search_pair;

private:

  constexpr size_t npos() const
  {
    return -1;
  }
  
  data_ptr create_(size_t size) const
  {
    if ( create_ )
      return create_(size);
    return std::make_unique<data_type>(size);
  }
  
  void free_(data_ptr d) const
  {
    if ( free_ )
      free_( std::move(d) );
  }

private:

// options
  sep_ptr _sep;
  size_t _sep_size;
  create_fun _create;
  free_fun _free;

// buffers
  data_ptr    _inbuf;
  data_ptr    _wait;
  buffer_list   _outlist;

// state
  size_t _wait_offset;
  size_t _search_buffer;
  size_t _search_offset;

};

}}
