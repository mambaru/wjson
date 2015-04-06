#pragma once

#include <iow/memory.hpp>
#include <fas/typemanip/empty_type.hpp>
#include <queue>
#include <memory>
#include <algorithm>
#include <boost/concept_check.hpp>

namespace iow{ namespace io{
  
/*
template<typename SepType>
struct read_buffer_options
{
  SepType sep;
};


template<typename DataType, typename SepType>
class read_buffer
{
public:
  typedef read_buffer_options<SepType> options_type;
  typedef std::shared_ptr<options_type> options_ptr;
  typedef DataType data_type;
  typedef typename data_type::value_type value_type;
  typedef value_type* value_ptr;
  typedef std::unique_ptr<data_type> data_ptr;
  typedef std::pair<value_ptr, size_t> data_pair;
public:

  read_buffer()
    : _offset()
    , _offbuf()
  {}
  
  void set_options(options_ptr options) noexcept
  {
    _options = options;
  }

  options_ptr get_options() const noexcept
  {
    return _options;
  }
  
  size_t count() const
  {
    return (_outbuf != nullptr) + (_outlist!=nullptr ?  _outlist->size() : 0 );
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
    
    // Еще в ожидании
    if ( _wait != nullptr )
      return result;
    
    // Нужно дочитать в хвост исходящего буфера
    if ( _offset != 0 && false )
    {
      // Только кагда используеться сепаратор
      // TODO: проверку на размер буфера
      _wait = std::move( this->last_() );
      result.first = &_wait->front() + _offset;
      result.second = _wait->size() - _offset;
    }
    else if ( _inbuf != nullptr )
    {
      _wait = std::move( _inbuf );
      result.first = &_wait->front();
      result.second = _wait->size();
    }

    return result;
  }
  
  bool confirm(data_pair d)
  {
    if ( _wait == nullptr )
      return false;
    
    if ( &(_wait->front()) + _offset - d.first != 0 )
      return false;
    
    if ( _wait->size() < _offset + d.second )
      return false;
        
    _wait->resize( _offset + d.second );

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
    return true;
  }
  
  // Забрать распарсенный блок
  data_ptr detach()
  {
    if ( _outbuf == nullptr )
      return nullptr;
    
    if ( _options==nullptr || _options->sep.empty() )
    {
      auto result = std::move(_outbuf);
      switch_front_();
      return std::move(result);
    }
    
    // Используется сепаратор
    return parse_detach_();
  }

private:
  typedef std::deque<data_ptr> deque_list;
  typedef std::unique_ptr<deque_list> deque_ptr;
  typedef std::pair<size_t, size_t> search_pair;

private:
  
  constexpr size_t npos() const
  {
    return -1;
  }

  
  data_ptr parse_detach_()
  {
    data_ptr result = nullptr;
    search_pair p = this->search_();

    if ( p.first == 0)
    {
      // Все данные в _outbuf
      result = this->detach_first_(p);
    } 
    else if ( p.first != npos() )
    {
      result = this->detach_list_(p);
    }
    else
    {
      // TODO: Проверку на minbuf и _inbuf
      _offbuf = this->count();
      _offset = 0;
    }
    return std::move(result);
  }
  
  data_ptr detach_first_(search_pair p)
  {
    data_ptr result = nullptr;
    if ( p.second == _outbuf->size() )
    {
      result = std::move(_outbuf);
      if ( _offset!=0 )
      {
        std::copy( result->begin() + _offset, result->end(), result->begin() );
        result->resize( result->size() - _offset);
        _offset = 0;
      }
      switch_front_();
    }
    else
    {
      result = std::make_unique<data_type>( _outbuf->begin() + _offset, _outbuf->begin() + p.second);
      _offset += result->size();
    }
    return std::move(result);
  }

  data_ptr detach_list_(search_pair p)
  {
    size_t reserve = 0;
    for ( size_t i =0 ; i < p.first; ++i )
    {
      data_ptr& cur = this->get_by_i_(i);
      reserve += cur->size();
    }

    data_ptr d = std::make_unique<data_type>(reserve);
    d->clear();
    data_ptr& cur = this->get_by_i_(0);
    std::copy(cur->begin() + _offset, cur->end(), std::inserter(*d, d->end()));
    if ( p.first > 1 )
    {
      for ( size_t i =1 ; i < p.first-2; ++i )
      {
        data_ptr& cur = this->get_by_i_(i);
        std::copy(cur->begin(), cur->end(), std::inserter(*d, d->end()));
      }
      data_ptr& cur = this->get_by_i_(p.first-1);
      std::copy(cur->begin(), cur->begin() + p.second, std::inserter(*d, d->end()));
      // if ( 
      //  _offset = p.second
    }
    
    while (p.first != 1)
    {
      _outlist->pop_front();
      --p.first;
    }
    return std::move(d);
  }

  data_ptr& last_()
  {
    if ( _outlist!= nullptr && !_outlist->empty() )
      return _outlist->back();
    return _outbuf;
  }
  
  void switch_front_()
  {
    // Если задействован список
    // (м.б. задействован если используеться сепаратор)
    if ( _outlist!=nullptr && !_outlist->empty() )
    {
      _outbuf = std::move(_outlist->front());
      _outlist->pop_front();
    }
  }
  
private:

  search_pair search_()
  {
    auto& sep = _options->sep;
    size_t cnt = this->count();
    for ( size_t i = _offbuf; i < cnt; ++i )
    {
      data_ptr& cur = this->get_by_i_(i);
      auto itr = std::find( cur->begin() + _offset, cur->end(), sep.back() );
      if ( itr != cur->end() )
      {
        ++itr;
        _offbuf = 0;
        // TODO: if ( compare_sep_( i, itr) )
        return search_pair( i, std::distance(cur->begin(), itr ) );
      }
    }
    return search_pair(npos(), npos());
  }

private:
  data_ptr& get_by_i_(size_t pos)
  {
    if (pos == 0)
      return _outbuf;
    auto itr = _outlist->begin();
    std::advance( itr, pos-1);
    return *itr;
  }

private:
  options_ptr _options;
  
  // Входящий буфер чтения
  data_ptr    _inbuf;
  // Тейкущий буфер чтения (ожидает подтверждения)
  data_ptr    _wait;
  size_t      _offset;
  size_t      _offbuf;
  // Исходящий буфер чтения (подтвержден)
  data_ptr    _outbuf;
  // Список исходящих буферов
  // Используеться при парсинге, если за один проход не удалось рапарсить 
  deque_ptr   _outlist;
};
*/

}}
