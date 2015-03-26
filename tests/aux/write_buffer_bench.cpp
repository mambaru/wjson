#include <iow/io/aux/write_buffer.hpp>
#include <chrono>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>



typedef std::vector<char> data_type;
typedef ::iow::io::write_buffer<data_type>  write_buffer;
typedef write_buffer::data_ptr data_ptr;

int fd = -1;

void attach(write_buffer& buf, size_t count)
{
  const char str[] = "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
  for (size_t i=0; i < count; ++i )
  {
    buf.attach(std::make_unique<data_type>(str, str+sizeof(str)-1));
  }
}

void proccess(write_buffer& buf)
{
  auto d = buf.next();
  while ( d.first )
  {
    write(fd, d.first, d.second);
    buf.confirm(d);
    d = buf.next();
  }
}

void run(size_t total, size_t count, size_t bufsize, size_t minbuf, size_t maxbuf)
{
  write_buffer buf;
  if ( bufsize != 0 )
  {
    auto opt = std::make_shared<write_buffer::options_type>();
    opt->bufsize = bufsize;
    opt->minbuf = minbuf;
    opt->maxbuf = maxbuf;
    opt->fix();
    buf.set_options(opt);
  }
  
  auto start = std::chrono::high_resolution_clock::now();
  
  for (size_t i=0; i < total; ++i )
  {
    attach(buf, count);
    proccess(buf);
  }
  
  auto finish = std::chrono::high_resolution_clock::now();
  
  auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start).count();
  auto rate = total*count*1000000 / span ;
  
  std::cout << "run " << span << " " << rate << std::endl;
}

int main()
{
  fd = open("/dev/null", O_APPEND);
  std::cout << fd << std::endl;
  run(1000, 1000, 4096, 512, 4096*2);
}
