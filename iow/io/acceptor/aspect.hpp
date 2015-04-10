#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <iow/io/basic/aspect.hpp>
#include <iow/io/reader/aspect.hpp>
#include <iow/asio.hpp>
#include <iow/system.hpp>
#include <fas/aop.hpp>
#include <boost/concept_check.hpp>
#include <list>

namespace iow{ namespace io{ namespace acceptor{
 
template<typename ConnectionType>
struct context
{
  typedef ConnectionType connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  connection_ptr connection;
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
    typedef typename get_type<T>::connection_type connection_type;
    typedef typename get_type<T>::descriptor_type descriptor_type;
    //auto& cntx = t.get_aspect().template get<_context_>();
    return std::make_shared<connection_type>( descriptor_type(t.descriptor().get_io_service()) );
  }
};


typedef ::iow::io::descriptor::holder<
  fas::aspect<
    ::iow::io::basic::aspect<>::advice_list,
    fas::type< ::iow::io::descriptor::_descriptor_type_, ::iow::asio::ip::tcp::socket >
  >
> test_connection;

struct ad_confirm
{
  std::list< std::shared_ptr<test_connection> > tmp;
  template<typename T, typename P>
  void operator()(T& , P p)
  {
    tmp.push_back(p);
  }
};


struct aspect: fas::aspect<
  ::iow::io::basic::aspect<>::advice_list,
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
