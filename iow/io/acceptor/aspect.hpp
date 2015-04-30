#pragma once

// test_connection
#include <iow/ip/tcp/connection/connection.hpp>

#include <iow/io/acceptor/tags.hpp>
#include <iow/io/basic/aspect.hpp>
#include <iow/io/reader/aspect.hpp>
#include <iow/io/reader/asio/aspect.hpp>
#include <iow/io/writer/asio/aspect.hpp>
#include <iow/io/stream/aspect.hpp>
#include <iow/io/descriptor/stream/aspect.hpp>
#include <iow/asio.hpp>
#include <iow/system.hpp>
#include <fas/aop.hpp>
#include <boost/concept_check.hpp>
#include <list>

namespace iow{ namespace io{ namespace acceptor{

template<typename DescriptorHolder>
class holder_manager
{
  // TODO: сделать пул, и ограничения 
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
    
    std::cout << "holder_manager::attach size=" <<_holders.size() << std::endl;
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
    std::cout << "holder_manager::close size=" <<_holders.size() << std::endl;
  }
  
  void free(holder_ptr h)
  {
    h->reset();
  }
  
private:
  typedef std::map< io_id_type, holder_ptr> holder_map;
  
  holder_map _holders;
};

  
template<typename ConnectionType>
struct context
{
  typedef ConnectionType connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  typedef holder_manager<connection_type> manager_type;
  typedef std::shared_ptr<manager_type> manager_ptr;
  //connection_ptr connection;
  manager_ptr manager;
  
};

struct options
{
  std::string host;
  std::string port;
  int backlog = 1024;
};

struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, const O& opt)
  {
    typedef typename T::aspect::template advice_cast< _context_ >::type context_type;
    t.get_aspect().template get<_context_>().manager 
      = std::make_shared<typename context_type::manager_type>();
    
    // t.descriptor().listen();
    iow::asio::ip::tcp::resolver resolver( t.descriptor().get_io_service() );
    iow::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      opt.host, 
      opt.port
    });

    t.descriptor().open(endpoint.protocol());
    t.descriptor().set_option( iow::asio::ip::tcp::acceptor::reuse_address(true) );
    t.descriptor().bind(endpoint);
    t.descriptor().listen( opt.backlog );
  }
};


struct ad_accept_handler
{
  template<typename T, typename P>
  void operator()(T& t, P p, ::iow::system::error_code ec)
  {
    if ( !ec )
    {
      t.get_aspect().template get<::iow::io::reader::_complete_>()(t, std::move(p));
    }
    else
    {
      ///!! t.get_aspect().template get<_read_error_>()( t, std::move(d), ec );
    }
  }
};

struct ad_make_handler
{
  typedef std::function<void(::iow::system::error_code)> handler_type;
  
  template<typename T, typename P>
  handler_type operator()(T& t, const P& p) 
  {
    std::weak_ptr<T> wthis = t.shared_from_this();
    return t.wrap([wthis, p]( ::iow::system::error_code ec )
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_accept_handler_>()(*pthis, std::move(p), std::move(ec));
      }
    });
  }
};

struct ad_async_accept
{
  template<typename T, typename P, typename H>
  void operator()(T& t, P p, H handler)
  {
    t.descriptor().async_accept( p->descriptor(), std::move(handler) );
  }
};




struct ad_next
{
  template<typename T>
  struct get_type
  {
    typedef typename T::aspect::template advice_cast< _context_ >::type context_type;
    typedef typename context_type::connection_type connection_type;
    typedef typename context_type::connection_ptr connection_ptr;
    typedef typename connection_type::descriptor_type descriptor_type;
  };
  
  template<typename T>
  auto operator()(T& t)
    -> typename get_type<T>::connection_ptr
  {
    return t.get_aspect().template get<_context_>().manager->create( t.descriptor().get_io_service() );
  }
};

typedef ::iow::ip::tcp::connection::connection<> test_connection;

struct tmp_opt: ::iow::io::descriptor::stream::options
{
  
};

struct ad_confirm
{
  //std::list< std::shared_ptr<test_connection> > tmp;
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    typedef typename T::io_id_type io_id_type;
    tmp_opt opt;
    opt.incoming_handler = []( tmp_opt::data_ptr d, size_t id, tmp_opt::outgoing_handler_t callback)
    {
      std::cout << "handler! " << id << std::endl;
      callback( std::move(d) );
    };
    
    std::weak_ptr<T> wthis = t.shared_from_this();
    opt.shutdown_handler = t.wrap([wthis](io_id_type id)
    {
      if ( auto pthis = wthis.lock() )
      {
        std::cout << "shutdown !" << std::endl;
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_context_>().manager->close(id);
      }
    });

    
    
    /*!!!
    opt.reader.sep="\r\n";
    opt.writer.sep="";
    */
    std::cout << "start !" << std::endl;
    p->start(opt);
    t.get_aspect().template get<_context_>().manager->attach(p->get_id(), p);
    // tmp.push_back(p);
  }
};


struct aspect: fas::aspect<
  ::iow::io::basic::aspect<std::recursive_mutex>::advice_list,
  ::iow::io::reader::aspect,
  fas::advice< ::iow::io::reader::_next_, ad_next>,
  //fas::advice<_listen_, ad_listen>,
  fas::advice<_initialize_, ad_initialize>,
  fas::advice< ::iow::io::reader::_make_handler_, ad_make_handler>,
  fas::advice< ::iow::io::reader::_read_some_, ad_async_accept>,
  fas::advice< ::iow::io::reader::_confirm_,  ad_confirm>,
  fas::stub< ::iow::io::reader::_handler_>,
  fas::advice< _accept_handler_, ad_accept_handler>,
  fas::value< _context_, context<test_connection> >
>{};

  
}}}
