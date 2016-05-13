#pragma once

#include <fas/aop.hpp>
#include <iow/io/descriptor/mtholder.hpp>
#include <iow/logger/logger.hpp>
#include <iow/io/client/connection.hpp>
#include <iow/workflow/workflow.hpp>

#include <memory>
#include <cassert>
#include <iow/asio.hpp>

namespace iow{ namespace io{ namespace client{

template<typename Connection/*, typename A = fas::aspect<>*/ >
class client 
  : public Connection
  , public std::enable_shared_from_this< client<Connection/*, A*/> >
{
public:
  typedef Connection super;
  typedef client<Connection> self;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::descriptor_type descriptor_type;
  typedef ::iow::asio::io_service io_service_type;
  typedef typename super::mutex_type mutex_type;
  typedef typename super::outgoing_handler_type outgoing_handler_t;
  //typedef ::iow::asio::deadline_timer reconnect_timer;
  typedef std::vector< data_ptr > wait_data_t;
  
  client( io_service_type& io)
    : super(std::move(descriptor_type(io)) )
    , _started(false)
    , _connected(false)
    , _ready_for_write(false)
    , _reconnect_timeout_ms(0)
    , _wait_cursize(0)
    , _wait_maxsize(0)
    , _wait_wrnsize(0)
  {
    _workflow = std::make_shared< ::iow::workflow>(io, ::iow::queue_options() );
  }
  
  client( io_service_type& io, descriptor_type&& desc)
    : super(std::move(desc) )
    , _started(false)
    , _ready_for_write(false)
    , _reconnect_timeout_ms(0)
    , _wait_cursize(0)
    , _wait_maxsize(0)
    , _wait_wrnsize(0)
  {
    _workflow = std::make_shared< ::iow::workflow>(io, ::iow::queue_options() );
  }
  
  template<typename Opt>
  void start(Opt opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    IOW_LOG_DEBUG("Client connect start " << opt.addr << ":" << opt.port << "" )
    if ( _started ) return;
    if ( opt.workflow != nullptr )
        _workflow = opt.workflow;
    _started = true;
    _ready_for_write = false;
    _reconnect_timeout_ms = opt.reconnect_timeout_ms;
    _wait_maxsize = opt.wait_maxsize;
    _wait_wrnsize = opt.wait_wrnsize;

    this->initialize_(opt);
    super::connect_( *this, opt );
  }

  template<typename Opt>
  void connect(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::connect_( *this, std::forward<Opt>(opt) );
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
    
    if ( !_connected )
      return std::move(d);
    
    if ( d==nullptr )
      return nullptr;    

    if ( _ready_for_write && _outgoing_handler!=nullptr )
    {
      _outgoing_handler( std::move(d) );
    }
    else
    {
      if ( _wait_cursize >= _wait_maxsize )
      {
        IOW_LOG_ERROR("Client limit wait data size " << _wait_cursize << "( max: " << _wait_maxsize << ")" 
                      << " dropped: [" << d << "]")
        return std::move(d);
      }
      else
      {
        if ( _wait_cursize >= _wait_wrnsize )
        {
          IOW_LOG_WARNING("Client limit wait data size " << _wait_cursize << "( max: " << _wait_maxsize << ")")
        }
        _wait_cursize += d->size();
        _wait_data.push_back( std::move(d) );
      }
    }
    return nullptr;
  }
  
  // return d - если не смог принять, nullptr в случае успеха
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
  void client_start_(Opt&& opt)
  {
    super::start_(*this, opt.connection);
  }
  
  void startup_handler_(io_id_t, outgoing_handler_t outgoing)
  {
    _ready_for_write = true;
    _outgoing_handler = outgoing;
    _connected = true;
    _wait_cursize = 0;
    std::for_each(_wait_data.begin(), _wait_data.end(), [outgoing](data_ptr& d) 
    {
      outgoing(std::move(d));
    });
    _wait_data.clear();
  }
  
  template<typename OptPtr>
  void delayed_reconnect_(OptPtr popt)
  {
    _workflow->delayed_post( 
      std::chrono::milliseconds( this->_reconnect_timeout_ms ),
      [popt, this](){
        this->connect(*popt);
      }
    );
  }

  template<typename Opt>
  void initialize_(Opt& opt)
  {
    auto popt = std::make_shared<Opt>(opt);
    auto startup_handler  = popt->connection.startup_handler;
    auto shutdown_handler = popt->connection.shutdown_handler;
    auto connect_handler  = popt->connect_handler;
    auto error_handler    = popt->error_handler;

    std::weak_ptr<self> wthis = this->shared_from_this();
    popt->connect_handler = [wthis, connect_handler, popt]()
    {
      if ( connect_handler ) connect_handler();
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->client_start_(*popt);
      }
    };
    
    
    popt->error_handler = [wthis, error_handler, popt](::iow::system::error_code ec)
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
        //pthis->stop();
        // моментальный реконнект
        std::lock_guard<mutex_type> lk( pthis->mutex() );
        pthis->_connected = false;
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
        = [wthis]( data_ptr d, io_id_t /*io_id*/, outgoing_handler_t /*outgoing*/)
      {
        IOW_LOG_ERROR("Client incoming_handler not set [" << d << "]" )
      }; 
    }

    opt = *popt;
  }

/*
  time_t now_ms()
  {
    using namespace ::boost::posix_time;
    ptime time_t_epoch( ::boost::gregorian::date(1970,1,1)); 
    ptime now = microsec_clock::local_time();
    time_duration diff = now - time_t_epoch;
    return diff.total_milliseconds();
  }
*/

private:
  bool _started;
  bool _connected;
  bool _ready_for_write;
  time_t _reconnect_timeout_ms;
  // time_t _connect_time_ms;
  // reconnect_timer _reconnect_timer;
  outgoing_handler_t _outgoing_handler;

  size_t      _wait_cursize;  
  size_t      _wait_maxsize;
  size_t      _wait_wrnsize;
  wait_data_t _wait_data;
  std::shared_ptr< ::iow::workflow > _workflow;
};

}}}
