#pragma once

#include <memory>
#include <map>
#include <iow/asio.hpp>

namespace iow{ namespace io{ namespace descriptor{

template<typename DescriptorHolder>
class manager
{
public:

  typedef DescriptorHolder holder_type;
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef typename holder_type::descriptor_type descriptor_type;
  typedef typename holder_type::options_type options_type;
  typedef typename holder_type::io_id_type io_id_type;
  typedef ::iow::asio::io_service io_service;
  
  void attach(io_id_type id, holder_ptr h)
  {
    _holders[id] = h;
  }
  
  holder_ptr create(io_service& io)
  {
    // TODO: взять из пула
    return std::make_shared<holder_type>( descriptor_type(io) );
  }
  
  void close(io_id_type id)
  {
    auto itr = _holders.find(id);
    if ( itr != _holders.end() )
    {
      this->free(itr->second);
      _holders.erase(itr);
    }
    else
    {
      //Сказать что-то в лог
    }
  }
  
  void free(holder_ptr h)
  {
    h->reset();
  }
  
private:
  
  typedef std::map< io_id_type, holder_ptr> holder_map;
  
  holder_map _holders;
};

}}}
