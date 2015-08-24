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
//#include <iow/io/descriptor/stream/aspect.hpp>
#include <iow/io/descriptor/manager.hpp>
#include <iow/io/descriptor/ad_initialize.hpp>
#include <iow/asio.hpp>
#include <iow/system.hpp>
#include <fas/aop.hpp>
#include <boost/concept_check.hpp>
#include <list>



namespace iow{ namespace io{ namespace acceptor{

struct ad_initialize
{
  template<typename T, typename O>
  void operator()(T& t, const O& opt)
  {
    t.get_aspect().template get<_context_>().connection_options = opt.connection;
    // TODO: убрать в TCP и сделать after_start
    typedef typename T::aspect::template advice_cast< _context_ >::type context_type;
    context_type& context = t.get_aspect().template get<_context_>();
    context.manager = std::make_shared<typename context_type::manager_type>();
    context.addr = opt.addr;
    context.port = opt.port;
    context.backlog = opt.backlog;

    /*
    iow::asio::ip::tcp::resolver resolver( t.descriptor().get_io_service() );
    iow::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      context.addr, 
      context.port
    });

    // такая же последовательность для local::stream_protocol::acceptor
    t.descriptor().open(endpoint.protocol());
    t.descriptor().set_option( iow::asio::ip::tcp::acceptor::reuse_address(true) );
    t.descriptor().bind(endpoint);
    t.descriptor().listen( opt.backlog );
    */
  }
};

/*
struct _set_reuse_address_;
struct _sync_resover_;

struct ad_sync_resover
{
  template<typename T>
  iow::asio::ip::tcp::endpoint operator()(T& t)
  {
    const auto& context = t.get_aspect().template get<_context_>();
    iow::asio::ip::tcp::resolver resolver( t.descriptor().get_io_service() );
    iow::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      context.addr, 
      context.port
    });
    return endpoint;
  }
};
*/

struct ad_resolve_and_start
{
  template<typename T>
  void operator()(T& t)
  {
    /*
    const auto& context = t.get_aspect().template get<_context_>();
    iow::asio::ip::tcp::resolver resolver( t.descriptor().get_io_service() );
    iow::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      context.addr, 
      context.port
    });*/
    auto endpoint = t.get_aspect().template get<_sync_resover_>()(t);
    t.get_aspect().template get<_acceptor_start_>()(t, endpoint);
  }
};

struct ad_acceptor_start
{
  template<typename T, typename Endpoint>
  void operator()(T& t, Endpoint endpoint)
  {
    const auto& context = t.get_aspect().template get<_context_>();
    // такая же последовательность для local::stream_protocol::acceptor
    t.descriptor().open(endpoint.protocol());
    t.get_aspect().template gete<_set_acceptor_options_>()(t);
    t.descriptor().bind(endpoint);
    t.descriptor().listen( context.backlog );
  }
};

// в tcp
/*
struct ad_set_reuse_address
{
  template<typename T>
  void operator()(T& t)
  {
    // такая же для local::stream_protocol::acceptor
    t.descriptor().set_option( iow::asio::ip::tcp::acceptor::reuse_address(true) );
  }
};
*/

struct aspect_base: fas::aspect<
  ::iow::io::basic::aspect<std::recursive_mutex>::advice_list,
  ::iow::io::reader::aspect,
  
  fas::advice< ::iow::io::reader::_next_, ad_next>,
  fas::advice< ::iow::io::_initialize_, ad_initialize>,
  fas::advice< ::iow::io::reader::_make_handler_, ad_make_handler>,
  fas::advice< ::iow::io::reader::_read_some_, ad_async_accept>,
  fas::advice< ::iow::io::reader::_confirm_,  ad_confirm>,
  fas::stub<   ::iow::io::reader::_handler_>,
  fas::advice< _accept_handler_, ad_accept_handler>,

#warning TODO: часть в tcp 
  //fas::advice< _sync_resover_, ad_sync_resover>,
  fas::group<  ::iow::io::_after_start_, _resolve_and_start_>,
  fas::advice< _resolve_and_start_, ad_resolve_and_start>,
  fas::advice< _acceptor_start_, ad_acceptor_start>// ,
  //fas::group< _set_acceptor_options_, _set_reuse_address_>,
  //fas::advice< _set_reuse_address_, ad_set_reuse_address>
>{};

template<typename ConnectionType>
struct aspect: fas::aspect<
  aspect_base::advice_list,
  fas::value< _context_, context<ConnectionType> >
>{};


}}}
