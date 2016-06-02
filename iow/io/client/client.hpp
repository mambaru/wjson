#pragma once

#include <fas/aop.hpp>

#include <iow/logger/logger.hpp>
#include <iow/io/client/connection.hpp>
#include <iow/workflow/workflow.hpp>

#include <memory>
#include <cassert>
#include <iow/asio.hpp>

namespace iow{ namespace io{ namespace client{

template<typename Connection >
class client 
  : public Connection
  , public std::enable_shared_from_this< client<Connection> >
{
public:
  typedef Connection super;
  typedef client<Connection> self;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::descriptor_type descriptor_type;
  typedef ::iow::asio::io_service io_service_type;
  typedef typename super::mutex_type mutex_type;
  typedef typename super::outgoing_handler_type outgoing_handler_t;
  typedef std::vector< data_ptr > wait_data_t;
  
  client( io_service_type& io)
    : super(std::move(descriptor_type(io)) )
    , _started(false)
    , _ready_for_write(false)
    , _reconnect_timeout_ms(0)
  {
  }
  
  client( io_service_type& io, descriptor_type&& desc)
    : super(std::move(desc) )
    , _started(false)
    , _ready_for_write(false)
    , _reconnect_timeout_ms(0)
  {
    //_workflow = std::make_shared< ::iow::workflow >(io, ::iow::queue_options() );
  }
  
  template<typename Opt>
  void start(Opt opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    IOW_LOG_MESSAGE("Client connect start " << opt.addr << ":" << opt.port << "" )
    if ( _started ) return;
    if ( opt.args.workflow != nullptr )
    {
        _workflow = opt.args.workflow;
    }
    else
    {
      IOW_LOG_WARNING("iow::io::client workflow not set")
    }
    _started = true;
    _ready_for_write = false;
    _reconnect_timeout_ms = opt.reconnect_timeout_ms;

    this->upgrate_options_(opt);
    super::connect_( *this, opt );
  }

  template<typename Opt>
  void connect(Opt opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    this->upgrate_options_(opt);
    super::connect_( *this, /*std::forward<Opt>(*/opt/*)*/ );
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    if ( !_started ) 
      return;
    _started = false;
    _ready_for_write = false;
    super::stop_(*this);
  }

  template<typename Handler>
  void shutdown(Handler&& handler)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::shutdown_( *this, std::forward<Handler>(handler) );
  }

  void close()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    _ready_for_write = false;
    super::close_(*this);
  }

  data_ptr send(data_ptr d)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    
    if ( d==nullptr )
      return nullptr;    

    if ( _ready_for_write && _outgoing_handler!=nullptr )
    {
      _outgoing_handler( std::move(d) );
    }
    else
    {
      IOW_LOG_ERROR("Client not conected. Not send: [" << d << "]")
      return std::move(d);
    }
    return nullptr;
  }
  
  void send( data_ptr d, io_id_t , outgoing_handler_t handler)
  {
    auto dd = this->send( std::move(d) ) ;
    if ( dd!=nullptr && handler!=nullptr )
    {
      handler(nullptr);
    }
  }

private:
  
  template<typename Opt>
  void client_start_(Opt opt)
  {
    this->upgrate_options_(opt);
    super::start_(*this, opt.connection);
  }

  
  void client_stop_()
  {
    super::stop_(*this);
    _ready_for_write = false;
    _outgoing_handler = nullptr;
  }
  
  void startup_handler_(io_id_t, outgoing_handler_t handler)
  {
    _ready_for_write = true;
    _outgoing_handler = handler;
  }
  
  template<typename Opt>
  void delayed_reconnect_(Opt opt)
  {
    _workflow->post( 
      std::chrono::milliseconds( this->_reconnect_timeout_ms ),
      [opt, this]() mutable
      {
        this->upgrate_options_(opt);
        this->connect_( *this, opt );
      }
    );
  }
  
  template<typename Opt>
  void upgrate_options_(Opt& opt)
  {
    Opt opt2 = opt;
    std::weak_ptr<self> wthis = this->shared_from_this();
    opt.args.connect_handler = this->wrap([wthis, opt2]()
    {
      if ( opt2.args.connect_handler!=nullptr ) 
        opt2.args.connect_handler();

      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->client_start_(opt2);
      }
    });

    opt.args.error_handler = this->wrap([wthis, opt2](::iow::system::error_code ec)
    {
      IOW_LOG_MESSAGE("iow::io::client error handler" )
      
      if ( opt2.args.error_handler!=nullptr ) 
        opt2.args.error_handler(ec);
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->_ready_for_write = false;
        pthis->delayed_reconnect_(opt2);
      }
    });
    
    opt.connection.shutdown_handler = this->wrap([wthis, opt2]( io_id_t io_id) 
    {
      IOW_LOG_MESSAGE("iow::io::client connection shutdown handler" )
      if ( opt2.connection.shutdown_handler!=nullptr ) 
        opt2.connection.shutdown_handler(io_id);
      
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->_ready_for_write = false;
        pthis->delayed_reconnect_(opt2);
      }
    });

    opt.connection.startup_handler = [wthis, opt2]( io_id_t io_id, outgoing_handler_t outgoing)
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->startup_handler_(io_id, outgoing);
      }

      if ( opt2.connection.startup_handler != nullptr )
      {
        opt2.connection.startup_handler( io_id, outgoing);
      }
    };

    if ( opt2.connection.incoming_handler == nullptr )
    {
      opt2.connection.incoming_handler
        = [wthis]( data_ptr d, io_id_t /*o_id*/, outgoing_handler_t /*outgoing*/)
      {
        IOW_LOG_ERROR("Client incoming_handler not set [" << d << "]" )
      }; 
    }

    /*
    auto popt = std::make_shared<Opt>(opt);
    auto startup_handler  = opt.connection.startup_handler;
    auto shutdown_handler = opt.connection.shutdown_handler;
    auto connect_handler  = opt.args.connect_handler;
    auto error_handler    = opt.args.error_handler;

    std::weak_ptr<self> wthis = this->shared_from_this();
    popt->args.connect_handler = [wthis, connect_handler, popt]()
    {
      if ( connect_handler ) connect_handler();
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->client_start_(*popt);
      }
    };
    
    popt->args.error_handler = [wthis, error_handler, popt](::iow::system::error_code ec)
    {
      if ( error_handler!=nullptr ) error_handler(ec);
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->delayed_reconnect_(popt);
      }
    };
    
    popt->connection.shutdown_handler = [wthis, shutdown_handler, popt]( io_id_t io_id) 
    {
      if ( shutdown_handler!=nullptr ) shutdown_handler(io_id);
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->_ready_for_write = false;
        pthis->delayed_reconnect_(popt);
      }
    };

    popt->connection.startup_handler = [wthis, startup_handler]( io_id_t io_id, outgoing_handler_t outgoing)
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->startup_handler_(io_id, outgoing);
      }

      if ( startup_handler != nullptr )
      {
        startup_handler( io_id, outgoing);
      }
    };
      
    if ( popt->connection.incoming_handler == nullptr )
    {
      popt->connection.incoming_handler
        = [wthis]( data_ptr d, io_id_t o_id, outgoing_handler_t outgoing)
      {
        IOW_LOG_ERROR("Client incoming_handler not set [" << d << "]" )
      }; 
    }

    opt = *popt;
    */
  }
private:
  bool _started;
  bool _ready_for_write;
  time_t _reconnect_timeout_ms;
  outgoing_handler_t _outgoing_handler;
  std::shared_ptr< ::iow::workflow > _workflow;
};

}}}
