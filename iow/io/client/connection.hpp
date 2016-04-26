#pragma once

#include <iow/io/connection/connection.hpp>


namespace iow{ namespace io{ namespace client{
  
struct _connect_;
struct _sync_resolver_;

struct ad_connect
{
  template<typename T, typename Opt>
  void operator()(T& t, Opt&& opt)
  {
    const auto endpoint = t.get_aspect().template get<_sync_resolver_>()(t, opt);
    auto ch = opt.connect_handler;
    auto er = opt.error_handler;
    std::weak_ptr<T> wthis = t.shared_from_this();
    
    auto tmp = t.wrap([wthis, opt](const ::iow::system::error_code& ec)
    {
      if ( auto p = wthis.lock() )
      {
        if (!ec)
        {
          IOW_LOG_END("Client connected to " << opt.addr << ":" << opt.port )
          if ( opt.connect_handler )
          {
            opt.connect_handler();
          }
        }
        else
        {
          IOW_LOG_END("Client FAIL connected to " << opt.addr << ":" 
                      << opt.port << ". " << ec.value() << " " << ec.message() )
          
          if ( opt.error_handler )
          {
            opt.error_handler(ec);
          }
        }
      }
    });


    #warning async_connect всегда возвращает OK. Поймать можно только при попытке прочитать
    ::iow::system::error_code ec;
    IOW_LOG_BEGIN("Client connect to " << opt.addr << ":" << opt.port << " ..." )
    t.descriptor().connect( endpoint, ec);
    tmp(ec);
    
    //t.descriptor().async_connect(endpoint, tmp);
  }
};

typedef fas::aspect< 
  fas::advice<_connect_, ad_connect>
> aspect;

template<typename A = fas::aspect<> >
class connection_base
  : public ::iow::io::connection::connection_base< typename fas::merge_aspect<A, aspect>::type >
{
public:
  typedef connection_base<A> self;
  typedef ::iow::io::connection::connection_base< typename fas::merge_aspect<A, aspect>::type > super;
  typedef typename super::descriptor_type descriptor_type;
  typedef typename super::data_ptr data_ptr;
  
  connection_base(descriptor_type&& desc)
    : super( std::move( desc ) )
  {}
  
  template<typename T, typename Opt>
  void connect_(T& t, Opt&& opt)
  {
    this->get_aspect().template get<_connect_>()(t, std::forward<Opt>(opt) );
  }
};

template<typename A = fas::aspect<> >
class connection
  : public connection_base<A>
  , public std::enable_shared_from_this< connection<A> >
{
public: 
   typedef connection_base<A> super;
   typedef typename super::descriptor_type descriptor_type;
   typedef typename super::mutex_type mutex_type;
  
  connection(descriptor_type&& desc)
    : super( std::move( desc ) )
  {}
  
  
  template<typename Opt>
  void connect(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::connect_( *this, std::forward<Opt>(opt) );
  }

  
  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::start_(*this, opt.connection);
  }

  void close()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::close_(*this);
  }

  void stop()
  {
    std::lock_guard<mutex_type> lk( super::mutex() );
    super::close_(*this);
  }
};

// удалить, плохая концепция
template<typename Connection = connection<> >
class multi_connection
{
  
public: 
  typedef Connection connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  typedef std::vector<connection_ptr> connection_list;
  typedef typename connection_type::descriptor_type descriptor_type;
  typedef iow::asio::io_service                     io_service_type;
  typedef std::mutex mutex_type;
  
  multi_connection( io_service_type& io)
    : _io(io) {}


  template<typename Opt>
  void connect(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( _mutex );
    this->connect_( *this, std::forward<Opt>(opt) );
  }

  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk( _mutex );
    this->start_( *this, std::forward<Opt>(opt) );
  }

  void close()
  {
    std::lock_guard<mutex_type> lk( _mutex );
    this->close_( *this );
  }

    
  template<typename T, typename Opt>
  void start_(T& t, const Opt& opt)
  {
    this->stop_(t);
    
    for (auto& conn : _connections)
    {
      conn->start(opt);
    }
  }

  template<typename T, typename Opt>
  void connect_(T& t, const Opt& opt)
  {
    this->close_(t);
    _connections.resize(opt.connect_count);
    for (auto& conn : _connections)
    {
      conn = std::make_shared<connection_type>( descriptor_type(_io) );
      conn->connect(opt);
    }
  }

  template<typename T>
  void close_(T&)
  {
    for (auto& conn : _connections)
      conn->close();
    _connections.clear();
  }

  template<typename T>
  void stop_(T&)
  {
    for (auto& conn : _connections)
      conn->stop();
  }
  
private:
  io_service_type& _io;
  mutex_type _mutex;
  connection_list _connections;
  
};

  
}}}
