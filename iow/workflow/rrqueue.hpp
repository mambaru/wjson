#pragma once 

#include <boost/circular_buffer.hpp>

namespace iow{
 
template<class T, class Allocator = std::allocator<T> >
using circular_buffer = ::boost::circular_buffer<T, Allocator>;

/*
template<class T, class Allocator = std::allocator<T> >
class rrqueue: std::vector<T, Allocator>
{
  typedef std::vector<T, Allocator> super;
  
public:
  
  typedef typename super::size_type size_type;
  typedef typename super::iterator iterator;

    
  using super::empty;
  //using super::size;
  using super::reserve;
  
  
  rrqueue(size_t max_size = 0 )
    : _beg(0)
    , _end(0)
  {}
  
  size_type size() const 
  {
    return _beg < _end
      ? _end - _beg
      : super::size() - (_beg - _end); 
    
  }
  size_type max_size() const { return _max_size;}
  void max_size(size_type max_size) { return _max_size = max_size;}
  
  void push_back( const T& value )
  {
    if ( _max_size < super::size() )
    {
      super::push_back(value);
    }
    else
    {
      super::at( next_() ) = value;
    }
  }

  void push_back( T&& value )
  {
    if ( _max_size < super::size() )
    {
      super::push_back(value);
    }
    else
    {
      super::at( next_() ) = value;
    }
  }

private:
  
  size_type next_()
  {
    if ( _end == _beg && _beg > 0 )
    {
      // size() == _max_size и мы где-то в середине
      _end = ++_beg;
      if ( _end == _max_size)
        _end = _beg = 0;
      return _beg;
    }
    else if ( _beg < _end )
    {
      
    }
  }
  
private:
  size_type _max_size;
  // За последним (при заполненой очереди - это первый элемент)
  size_type _beg; // первый
  size_type _end; // за последним
};
*/


}