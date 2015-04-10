#include <iostream>
#include <iow/io/descriptor/holder.hpp>
#include <iow/io/acceptor/aspect.hpp>
#include <iow/asio.hpp>

typedef ::iow::io::descriptor::holder< fas::aspect<
  ::iow::io::acceptor::aspect::advice_list,
  fas::type< ::iow::io::descriptor::_descriptor_type_, iow::asio::ip::tcp::acceptor >
> > tcp_acceptror;

typedef ::iow::io::acceptor::options options;

int main()
{
  typedef tcp_acceptror::descriptor_type accept_descriptor;
  iow::asio::io_service io_service; 
  accept_descriptor desc = accept_descriptor(io_service);
  auto acceptor = std::make_shared<tcp_acceptror>( std::move(desc)  );
  options opt;
  opt.host = "0.0.0.0";
  opt.port = "12345";
  acceptor->start(opt);
  io_service.run();
}