
#include <iostream>
#include <iow/ip/tcp/server/server.hpp>
#include <iow/ip/tcp/server/options.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <thread>
#include <mutex>

iow::asio::io_service io_service; 
std::atomic<int> connect_count;
void server()
{
  typedef ::iow::ip::tcp::server::server<> tcp_server;
  typedef ::iow::ip::tcp::server::options<> options;
  
  auto acceptor = std::make_shared<tcp_server>( io_service  );
  options opt;
  opt.addr = "0.0.0.0";
  opt.port = "12345";
  
  std::cout << "server start..." << std::endl;
  acceptor->start(opt);
  std::cout << "server run..." << std::endl;
  io_service.run();
  std::cout << "server done" << std::endl;
}


int main()
{
  connect_count = 1;

  typedef ::iow::ip::tcp::client::client<> client_type;
  typedef ::iow::ip::tcp::client::options options_type;
  
  auto tcp_client = std::make_shared<client_type>(io_service);
  
  
  options_type opt;
  opt.connect_count = connect_count;
  opt.args.connect_handler = []()
  {
    std::cout << "connect ready" << std::endl;
    --connect_count;
    if ( connect_count==0 ) 
    {
      io_service.post([]()
      {
        std::cout << "stop io ..." << std::endl; 
        sleep(1);
        io_service.stop();
        std::cout << "stop io ... ready" << std::endl; 
      }); 
    }
  };
  opt.addr = "0.0.0.0";
  opt.port = "12345";
  opt.reconnect_timeout_ms = 1000;
  opt.connection.incoming_handler=[](iow::io::data_ptr, iow::io::io_id_t, ::iow::io::outgoing_handler_t)
  {
    
  };
  opt.connection.reader.trimsep=true;
  // std::cout << "client connect..." << std::endl;
  //tcp_client->connect(opt);
  std::cout << "client start..." << std::endl;
  tcp_client->start(opt);
  tcp_client->send( iow::io::make("Hello World!") );
  std::thread t(server);
  std::cout << "client main run..." << std::endl;
  io_service.run();
  if ( connect_count != 0 ) 
  {
    std::cout << "TEST FAIL" << std::endl;
    std::cout.flush();
    abort();
  }
  else 
  {
    std::cout << "OK" << std::endl;
  }
  
  
  
  
  /*
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
  */
 
  t.join();
  return 0;
}