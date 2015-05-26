#pragma once


#include <iow/ip/tcp/server/options.hpp>
#include <iow/io/basic/tags.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace ip{ namespace tcp{ namespace server{

struct _context_;

template<typename AcceptorType>
struct context
{
  typedef AcceptorType acceptor_type;
  typedef std::shared_ptr<acceptor_type> acceptor_ptr;
  
  acceptor_ptr acceptor;
};

struct ad_create_acceptor
{
  template<typename T>
  void operator()( T& t )
  {
    typedef typename T::aspect::template advice_cast<_context_>::type context_type;
    typedef typename context_type::acceptor_type acceptor_type;
    typedef typename acceptor_type::descriptor_type descriptor_type;
    
    auto& cntx = t.get_aspect().template get<_context_>();
    cntx.acceptor = std::make_shared<acceptor_type>( descriptor_type( t.get_io_service() ) );
  }
};
  
template<typename AcceptorType = acceptor>
struct aspect: fas::aspect<
  fas::type< _context_, context<AcceptorType> >
>{};
  
}}}}
