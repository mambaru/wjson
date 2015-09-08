#pragma once

#include <iow/io/descriptor/mtdup.hpp>
#include <iow/logger/logger.hpp>

#include <memory>

namespace iow{ namespace io{ namespace client{

/*
 * потребитель регистрирует с io_id_t и outgoing_handler_t
 * регистрация при старте (или автоматически при первом обращении)
 * для каждого потребителя свой коннект (несколько?) и пул потоков
 *
 */


class outgoing_map
{
public:
  typedef ::iow::io::io_id_t io_id_t;
  typedef ::iow::io::outgoing_handler_t outgoing_handler_t;
  typedef ::iow::io::data_ptr data_ptr;

  outgoing_map()
  {
    _iterator = _remotes.end();
  }

  void set( io_id_t io_id, outgoing_handler_t handler)
  {

  }

  outgoing_handler_t get(io_id_t io_id) const
  {

  }

  void send(data_ptr d, io_id_t io_id, outgoing_handler_t&& handler)
  {
    if ( handler == nullptr )
    {
      return this->send_(std::move(d), std::forward<outgoing_handler_t>(handler) );
    }

    return this->send_(std::move(d), []( data_ptr d )
    {
      
    });
  }

private:

  bool send_(data_ptr d, outgoing_handler_t&& handler)
  {
    if ( _remotes.empty() )
    {
      return false;
    }

    if ( _iterator == _remotes.end() )
    {
      _iterator = _remotes.begin();
    }

    _iterator->second( std::move(d) );

    ++_iterator;
  }

  typedef std::map< io_id_t, outgoing_handler_t> handler_map;
  typedef handler_map::const_iterator const_iterator;
  handler_map _remotes;
  handler_map _sources;
  const_iterator _iterator;
};

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
  typedef std::map<
    ::iow::io::io_id_t, 
    ::iow::io::outgoing_handler_t
  > handler_map;
  
  ::iow::io::outgoing_handler_t g_tmp = nullptr;
  ::iow::io::outgoing_handler_t g_tmp2 = nullptr;

  
  template<typename Opt>
  void update_options_(Opt& opt)
  {
    std::weak_ptr<self> wthis = this->shared_from_this();
    auto startup_handler = opt.connection.startup_handler;
    opt.connection.startup_handler = [wthis, startup_handler]( ::iow::io::io_id_t io_id, ::iow::io::outgoing_handler_t outgoing)
    {
      IOW_LOG_MESSAGE("Connected!!! 2");
      if ( auto pthis = wthis.lock() )
      {
        pthis->g_tmp = outgoing;
      }

      if ( startup_handler != nullptr )
      {
        startup_handler( io_id, outgoing);
      }
    };
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
