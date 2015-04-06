#include <iostream>
#include <iow/io/aux/read_buffer.hpp>
#include <iow/io/aux/data_pool.hpp>
#include <chrono>

#include <fcntl.h>
#include <unistd.h>


#ifndef NDEBUG
#define TOTAL 1
#else
#define TOTAL 1000
#endif

typedef std::vector<char> data_type;
typedef ::iow::io::read_buffer<data_type>  read_buffer;
typedef ::iow::io::data_pool<data_type>  data_pool;
typedef read_buffer::data_ptr data_ptr;


void run(size_t packsize, size_t readsize, size_t total, size_t count, size_t bufsize, size_t minbuf, size_t maxbuf, bool use_pool)
{
  std::cout << "*****************************************" << std::endl;
  data_pool::options_type opt;
  opt.poolsize = 10;
  opt.maxbuf = maxbuf;
  opt.minbuf = maxbuf;
  data_pool pool;
  pool.set_options(opt);
  
  read_buffer buf;
  if ( bufsize != 0 )
  {
    read_buffer::options_type opt;
    buf.get_options(opt);
    opt.bufsize = bufsize;
    opt.minbuf = minbuf;
    opt.maxbuf = maxbuf;
    opt.sep = "\r\n";
    if ( use_pool )
    {
      opt.create = std::bind( static_cast<data_ptr(data_pool::*)(size_t, size_t)>(&data_pool::create), std::ref(pool), std::placeholders::_1, std::placeholders::_2);
      opt.free = std::bind(&data_pool::free, std::ref(pool), std::placeholders::_1);
    }
    buf.set_options(opt);
    std::cout << "bufsize=" << opt.bufsize 
              << ", minbuf=" << opt.bufsize 
              << ", maxbuf=" << opt.maxbuf 
              << std::endl;
    std::cout << "Размер пакета: " << packsize << " 'чтение' за раз: " << readsize << std::endl;
  }
  
  
  size_t pack_count = 0;
  std::vector<char> indata;
  for (size_t i =0 ; i < count; ++i)
  {
    for (size_t j =0 ; j < packsize - 2; ++j)
      indata.push_back('0'+ (j)%packsize);
    indata.push_back('\r');
    indata.push_back('\n');
    ++pack_count;
  }
  
  std::cout << "start... " << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  
  size_t parse_pack = 0;
  size_t read_block = 0;
  for (size_t i=0; i < total; ++i )
  {
    size_t pos = 0;
    while ( pos < indata.size() )
    {
      auto p = buf.next();
      if (p.second + pos > indata.size())
        p.second = indata.size() - pos;
      if ( p.second > readsize )
        p.second = readsize;
      
      ++read_block;
      std::copy( 
        indata.begin() + pos, 
        indata.begin() + pos + p.second,
        p.first
      );
      buf.confirm(p);
      
      /*if ( buf.count() > 1 )
      {
        std::cout << "count " << buf.count() << std::endl;
      }*/
      auto d = buf.detach();
      while ( d!=nullptr )
      {
        if ( d->size() != packsize || !std::equal(d->begin(), d->end(), indata.begin()) )
        {
          std::cout << "Error: [[" << std::string(d->begin(), d->end()) << "]] " << d->size() << std::endl; 
          std::cout << "pos: " << pos << std::endl; 
          std::cout << "i: " << i << std::endl;
          std::cout << "parse packs: " << parse_pack << std::endl;
          std::cout << "blocks: " << read_block << std::endl;
          abort();
        }
        ++parse_pack;
        pool.free( std::move(d) );
        d = buf.detach();
      }
      pos += p.second;
    }
  }
  
  auto finish = std::chrono::high_resolution_clock::now();
  
  auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start).count();
  
  std::cout << "time: " << span << " microseconds" << std::endl;
  std::cout << "reads blocks: " << read_block << std::endl;
  std::cout << "expected packs: " << pack_count * total << std::endl;
  std::cout << "parse packs: " << parse_pack << std::endl;
  std::cout << "read rate: " << read_block*1000000 / span  << std::endl;
  std::cout << "parse rate: " << parse_pack*1000000 / span  << std::endl;
}

int main()
{
  run(10,  1,  TOTAL, 1000, 1, 1, 1, false);
  run(10,  10, TOTAL, 1000, 10, 10, 10, false);
  run(10,  10, TOTAL, 1000, 10, 5, 15, false);
  run(10,  10, TOTAL, 1000, 128, 128, 256, false);
  run(128, 10, TOTAL, 1000, 10, 128, 128, false);
  run(128, 10, TOTAL, 1000, 10, 10, 10, true);
  run(128, 10, TOTAL, 1000, 10, 10, 10, false);
  run(128, 10, TOTAL, 1000, 4096, 4096, 4096, true);
  run(128, 10, TOTAL, 1000, 4096, 4096, 4096, false);
  run(128, 128, TOTAL, 1000, 4096, 1024, 4096*2, true);
}
