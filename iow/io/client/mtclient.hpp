#pragma once

#include <iow/io/descriptor/mtholder.hpp>
#include <iow/logger/logger.hpp>

namespace iow{ namespace io{ namespace client{

template<typename Client >
class mtclient
  : public ::iow::io::descriptor::mtholder<Client>
{
  typedef mtclient<Client> self;
  typedef ::iow::io::descriptor::mtholder<Client> super;
public:
  typedef typename super::io_service_type io_service_type;
  typedef typename super::data_ptr data_ptr;

  mtclient(io_service_type& io)
    : super(io)
  {
  }

  data_ptr send(data_ptr d)
  {
    if ( auto c = super::next() )
    {
      return std::move( c->send(std::move(d)) );
    }
    else
    {
      IOW_LOG_ERROR( "mtclient::send drop [" << d << "]" )
    }
    return nullptr;
  }

};

}}}
