
#include <iostream>
#include <iow/ip/tcp/acceptor/acceptor.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <thread>
#include <mutex>
void server()
{
  typedef ::iow::ip::tcp::acceptor::acceptor<> tcp_acceptor;
  typedef tcp_acceptor::descriptor_type accept_descriptor;
  typedef tcp_acceptor::options_type options;
  iow::asio::io_service io_service; 
  accept_descriptor desc = accept_descriptor(io_service);
  auto acceptor = std::make_shared<tcp_acceptor>( std::move(desc)  );
  options opt;
  opt.host = "0.0.0.0";
  opt.port = "12345";
  acceptor->start(opt);
  io_service.run();
}


int main()
{
  std::thread t(server);

  typedef ::iow::ip::tcp::client::client<> tcp_client;
  typedef tcp_client::descriptor_type accept_descriptor;
  typedef tcp_client::options_type options;
  iow::asio::io_service io_service; 
  accept_descriptor desc = accept_descriptor(io_service);
  auto client = std::make_shared<tcp_client>( std::move(desc)  );
  options opt;
  opt.host = "0.0.0.0";
  opt.port = "12345";
  client->start(opt);
  io_service.run();
 
  t.join();
}