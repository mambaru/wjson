#include <iostream>
#include <iow/io/descriptor/holder.hpp>
#include <iow/io/descriptor/stream/aspect.hpp>
#include <iow/io/reader/asio/aspect.hpp>
#include <iow/io/writer/asio/aspect.hpp>
#include <iow/io/stream/aspect.hpp>
#include <iow/io/basic/aspect.hpp>

#include <fas/testing.hpp>

typedef std::vector<char> data_type;
typedef iow::asio::posix::stream_descriptor descriptor_type;
typedef iow::io::descriptor::stream::options options_type;

struct aspect_stream: fas::aspect<
  fas::alias< ::iow::io::descriptor::_outgoing_, ::iow::io::writer::_output_>,
  fas::type< ::iow::io::descriptor::_descriptor_type_, iow::asio::posix::stream_descriptor>,
  ::iow::io::descriptor::stream::aspect,
  ::iow::io::reader::asio::aspect::advice_list,
  ::iow::io::writer::asio::aspect::advice_list,
  ::iow::io::stream::aspect< data_type >::advice_list, 
  ::iow::io::basic::aspect<std::recursive_mutex>::advice_list,
  fas::stub< ::iow::io::_initialize_ >,
  fas::type< ::iow::io::_options_type_, fas::empty_type >
  
>{};

typedef ::iow::io::descriptor::holder<aspect_stream> stream_holder;


UNIT(stream_holder_unit, "")
{
  using namespace fas::testing;
  
  
  iow::asio::io_service service;
  int f1[2]={-1, -1};
  int f2[2]={-1, -1};
  int res1 = ::pipe(f1);
  int res2 = ::pipe(f2);
  t << message("pipe1: ") << res1 << " " << f1[0] << " " << f1[1] << std::endl;
  t << message("pipe2: ") << res2 << " " << f2[0] << " " << f2[1] << std::endl;
  descriptor_type d1(service, f1[0]);
  descriptor_type d2(service, f2[1]);
  
  auto h1 = std::make_shared<stream_holder>(std::move(d1));
  auto h2 = std::make_shared<stream_holder>(std::move(d2));
  const char* instr = "Hello world!\r\n";
  t << message("write...");
  res1 = write(f1[1], instr, std::strlen(instr) );
  t << message("...write:") << res1;
  
  options_type opt;
  opt.incoming_handler = [&](options_type::data_ptr d, size_t, options_type::outgoing_handler_type /*callback*/){
    std::cout << std::endl << "yooooooooooo!!!" << std::endl;
    std::cout << std::endl << std::string(d->begin(), d->end() );
    h2->get_aspect().get< ::iow::io::writer::_output_>()( *h2, std::move(d) );
    // callback(std::move(d));
  };
  t << message("start1");
  h1->start(opt);
  t << message("start2");
  h2->initialize(opt);
  t << message("service.run()...");
  service.run();
  t << message("...service.run()");
  service.run();
  
  char outstr[128];
  int size = read(f2[0], outstr, 128);
  outstr[size]='\0';
  t << message("outstr: ") << outstr << std::endl;
  t << nothing;
  /*::iow::asio::io_service io;
  stream f(io);
  
  f.add("Hello ");
  f.add("world");
  f.add("!");
  f.start();
  io.run();
  t << equal<expect, std::string>(f.result, "Hello world!") << f.result << FAS_TESTING_FILE_LINE;
  */
  
}

BEGIN_SUITE(stream_holder,"")
  ADD_UNIT(stream_holder_unit)
END_SUITE(stream_holder)

