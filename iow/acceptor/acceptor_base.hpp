#pragma once

#include<iow/pipeline/descriptor_holder.hpp>

namespace iow{
  
struct _descriptor_type_;


struct aspect_acceptor_base: fas::aspect< 
  
> {};

template<typename A>
class acceptor_base
  : public descriptor_holder< typename fas::merge_aspect<A, aspect_acceptor_base>::type  >
{
public:
  typedef acceptor_base<A> self;
  typedef descriptor_holder<typename fas::merge_aspect<A, aspect_acceptor_base>::type> super;
  
};

namespace ip{ namespace tcp{
  
struct acceptor_options
{
  size_t backlog = 1024;
  //connection_options connection;
  std::string host /*= "0.0.0.0"*/;
  std::string port /*= "12345"*/;
};

struct aspect_acceptor
  : fas::aspect< 
      fas::type<_descriptor_type_, ::iow::asio::ip::tcp::acceptor >,
      fas::type<_options_type_, acceptor_options>
    > 
{};

template<typename A>
class acceptor final:
  public ::iow::acceptor_base< typename fas::merge_aspect< A, aspect_acceptor >::type > 
{
public:
  typedef acceptor<A> self;
  typedef ::iow::acceptor_base< typename fas::merge_aspect< A, aspect_acceptor >::type > super;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::options_type    options_type;
  typedef typename super::descriptor_type descriptor_type;
  
  /*
  acceptor(io_service_type& io, const options_type& opt)
    : super( io, opt)
  {
  }*/
  
  void listen()
  {
    // COMMON_LOG_MESSAGE("listen " << super::options().host << ":" << super::options().port)
    boost::asio::ip::tcp::resolver resolver( super::get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(
    {
      super::options().host, 
      super::options().port
    });

    super::descriptor().open(endpoint.protocol());
    super::descriptor().set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
    super::descriptor().bind(endpoint);
    super::descriptor().listen( super::options().backlog );
  }

  IOW_INHERITING_CONSTRUCTORS_T(acceptor, super)
  IOW_IO_BASE_IMPL_T(super)


};

}}
  
}