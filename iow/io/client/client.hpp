#pragma once

#include <iow/io/descriptor/mtdup.hpp>
#include <iow/logger/logger.hpp>

#include <memory>

namespace iow{ namespace io{ namespace client{
  
template<typename Connection>
class client
  : private ::iow::io::descriptor::mtdup<Connection>
  , public std::enable_shared_from_this< client<Connection> >
{
  typedef ::iow::io::descriptor::mtdup<Connection> super;
  typedef client<Connection> self;
  typedef boost::asio::deadline_timer reconnect_timer;
 
public:

  typedef typename super::io_service_type io_service_type;
  typedef typename super::mutex_type mutex_type;
  typedef Connection connection_type;
  typedef std::shared_ptr<connection_type> acceptor_ptr;
  typedef typename connection_type::descriptor_type descriptor_type;

  client(io_service_type& io)
    : super( std::move( descriptor_type(io) ) )
    , _reconnect_timer(io)
  {}

  template<typename Opt>
  void start(Opt&& opt)
  {
    this->update_options_(opt);
    
    _reconnect_timeout_ms = opt.reconnect_timeout_ms;
    auto connect_handler = opt.connect_handler;
    auto error_handler = opt.error_handler;
    std::weak_ptr<self> wthis = this->shared_from_this();
    opt.connect_handler = [connect_handler, wthis, opt](){
      IOW_LOG_MESSAGE("Client CONNECTED!!! ????")
      if ( connect_handler ) connect_handler();
      if ( auto pthis = wthis.lock() )
      {
        pthis->start_connection_(opt);
      }
    };

    opt.error_handler = [connect_handler](::iow::system::error_code ec){
      IOW_LOG_MESSAGE("Client CONNECT ERROR!!! " << ec.message())
    };

    super::origin()->connect(opt);
    /// super::start(opt);
  }

  void stop()
  {
    super::stop();
  }
  
private:
  
  template<typename Opt>
  void update_options_(Opt& opt)
  {
    std::weak_ptr<self> wthis = this->shared_from_this();
    auto startup_handler = opt.startup_handler;
    opt.startup_handler = [wthis]
    
  }
  
  // Вызываеться при коннеекте 
  void startup_handler_()
  {
    
  }
  
  template<typename Opt>
  void start_connection_(Opt&& opt)
  {
    super::start(opt);
  }

  reconnect_timer _reconnect_timer;
  time_t _reconnect_timeout_ms;

};

}}}
