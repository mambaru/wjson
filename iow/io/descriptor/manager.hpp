#pragma once

#include <memory>
#include <map>
#include <iow/asio.hpp>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace descriptor{

template<typename DescriptorHolder>
class manager
{
public:

  typedef DescriptorHolder holder_type;
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef typename holder_type::descriptor_type descriptor_type;
  //typedef typename holder_type::options_type options_type;
  typedef typename holder_type::io_id_type io_id_type;
  typedef ::iow::asio::io_service io_service;

  template<typename Opt>
  manager(Opt&& opt)
  {
    _initilizer = [opt](holder_ptr h)
    {
      h->initialize(opt);
    };
  }
  
  void attach(io_id_type id, holder_ptr h)
  {
    _holders[id] = h;
  }

  holder_ptr create(io_service& io)
  {
    auto h =  std::make_shared<holder_type>( descriptor_type(io) );
    this->_initilizer(h);
    return h;
  }

  holder_ptr create(descriptor_type&& desc)
  {
    auto h =  std::make_shared<holder_type>( std::forward<descriptor_type>(desc) );
    this->_initilizer(h);
    return h;
  }

  void erase(io_id_type id)
  {
    auto itr = _holders.find(id);
    if ( itr != _holders.end() )
    {
      _holders.erase(itr);
    }
    else
    {
      IOW_LOG_ERROR("iow::io::descriptor::manager::erase: id=" << id << " not found")
    }
  }

private:
  std::function<void(holder_ptr)> _initilizer;
  typedef std::map< io_id_type, holder_ptr> holder_map;
  holder_map _holders;
};

}}}
