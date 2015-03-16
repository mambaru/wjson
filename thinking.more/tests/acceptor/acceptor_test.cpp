#include <iostream>
#include <iow/acceptor/acceptor_base.hpp>
#include <fas/testing.hpp>

typedef fas::aspect< 
  fas::type< iow::_options_type_, fas::empty_type>,
  //fas::type< iow::_context_type_, fas::empty_type>,
  fas::type< iow::_mutex_type_, fas::empty_type>,
  fas::type< iow::_descriptor_type_, ::iow::asio::ip::tcp::acceptor>
> aspect_tmp;


UNIT(acceptor, "")
{
  using namespace fas::testing;
  //typedef ::iow::descriptor_holder< fas::merge_aspect< aspect_tmp, ::iow::aspect_acceptor_base /*, ::fas::aspect<>*/ >::type > acceptor;
  //typedef acceptor::options_type  options_type;
  typedef ::iow::ip::tcp::acceptor<> acceptor;
  //typedef ::iow::acceptor_base<aspect_tmp> acceptor;
  typedef acceptor::options_type  options;
  typedef acceptor::io_service_type  io_service;
  
  io_service io;
  options opt;
  auto acc = std::make_shared< ::iow::ip::tcp::acceptor<> >(io, opt);
  acc->listen();
  acc->start();
  io_service::work wrk(io);
  io.run();
  
  t << nothing;
}

BEGIN_SUITE(acceptor,"acceptor")
  ADD_UNIT(acceptor)
END_SUITE(acceptor)

BEGIN_TEST
  RUN_SUITE(acceptor)
END_TEST
