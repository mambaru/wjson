#pragma once

// test_connection
//#include <iow/ip/tcp/connection/connection.hpp>

#include <iow/io/acceptor/tags.hpp>
#include <iow/io/acceptor/context.hpp>
#include <iow/io/acceptor/options.hpp>
#include <iow/io/acceptor/ad_confirm.hpp>
#include <iow/io/acceptor/ad_accept_handler.hpp>
#include <iow/io/acceptor/ad_make_handler.hpp>
#include <iow/io/acceptor/ad_async_accept.hpp>
#include <iow/io/acceptor/ad_next.hpp>

#include <iow/io/basic/aspect.hpp>
#include <iow/io/reader/aspect.hpp>
#include <iow/io/reader/asio/aspect.hpp>
#include <iow/io/writer/asio/aspect.hpp>
#include <iow/io/stream/aspect.hpp>
#include <iow/io/descriptor/stream/aspect.hpp>
#include <iow/io/descriptor/manager.hpp>
#include <iow/asio.hpp>
#include <iow/system.hpp>
#include <fas/aop.hpp>
#include <boost/concept_check.hpp>
#include <list>



namespace iow{ namespace io{ namespace acceptor{

  /*
struct ad_confirm
{
  
  template<typename T, typename P>
  void operator()(T& t, P p)
  {
    typedef typename T::io_id_type io_id_type;
    typedef typename type_<T>::options_type options_type;
    options_type opt;
    
    opt.incoming_handler = []( 
      typename options_type::data_ptr d, 
      io_id_type, 
      typename options_type::outgoing_handler_t callback
    )
    {
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

    p->start(opt);
    t.get_aspect().template get<_context_>().manager->attach(p->get_id(), p);
  }

private:
  template<typename T>
  struct type_
  {
    typedef typename T::aspect::template  advice_cast<_context_>::type context_type;
    typedef typename context_type::connection_type connection_type;
    typedef typename connection_type::options_type options_type;
  };
};
*/

struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, const O& opt)
  {
    std::cout << "acceptor::ad_initialize sep=[" << opt.connection_options.reader.sep.size() << "]" << std::endl;
    t.get_aspect().template get<_context_>().connection_options = opt.connection_options;
    // TODO: убрать в TCP и сделать after_start
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

/*
struct ad_listen
{
  template<typename T, typename O>
  void operator()(T& t, const O& opt)
  {
  }
};
*/


struct aspect_base: fas::aspect<
  ::iow::io::basic::aspect<std::recursive_mutex>::advice_list,
  ::iow::io::reader::aspect,
  fas::advice< ::iow::io::reader::_next_, ad_next>,
  //fas::advice<_listen_, ad_listen>,
  fas::advice<_initialize_, ad_initialize>,
  fas::advice< ::iow::io::reader::_make_handler_, ad_make_handler>,
  fas::advice< ::iow::io::reader::_read_some_, ad_async_accept>,
  fas::advice< ::iow::io::reader::_confirm_,  ad_confirm>,
  fas::stub< ::iow::io::reader::_handler_>,
  fas::advice< _accept_handler_, ad_accept_handler>
>{};

template<typename ConnectionType>
struct aspect: fas::aspect<
  aspect_base::advice_list,
  fas::value< _context_, context<ConnectionType> >
>{};

/*template<typename ConnectionType, typename A = fas::empty_type>
class acceptor: 
  public ::iow::io::descriptor::holder< 
    typename fas::merge_aspect<A, aspect<ConnectionType> >::type 
  >
{
};
*/

/*
template <typename ConnectionType, typename A = fas::empty_type>
using acceptor = ::iow::io::descriptor::holder< typename fas::merge_aspect<A, aspect<ConnectionType> >::type >;
*/

/*typedef ::iow::io::descriptor::holder< fas::aspect<
  fas::type< ::iow::io::_options_type_, fas::empty_type >,
  ::iow::io::acceptor::aspect::advice_list,
  fas::type< ::iow::io::descriptor::_descriptor_type_, iow::asio::ip::tcp::acceptor >
> > tcp_acceptror;
*/

  
}}}
