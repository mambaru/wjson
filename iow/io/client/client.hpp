#pragma once

#include <iow/io/descriptor/mtdup.hpp>

namespace iow{ namespace io{ namespace client{
  
template<typename Connection>
class client
  : private ::iow::io::descriptor::mtdup<Connection>
{
  typedef ::iow::io::descriptor::mtdup<Connection> super;
  
public: 
  typedef typename super::io_service_type io_service_type;
  typedef Connection connection_type;
  typedef std::shared_ptr<connection_type> acceptor_ptr;
  typedef typename connection_type::descriptor_type descriptor_type;
  
  client(io_service_type& io)
    : super( std::move( descriptor_type(io) ) )
  {}
  
  template<typename Opt>
  void start(Opt&& opt)
  {
    super::origin()->connect(opt);
    super::start(opt);
  }

  void stop()
  {
    super::stop();
  }
};
  
}}}
