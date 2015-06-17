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
#define DATA_SIZE 50000

typedef std::vector<char> data_type;
typedef ::iow::io::read_buffer<data_type>  read_buffer;
typedef read_buffer::data_ptr data_ptr;

/*
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
*/
int main()
{
  data_type data(DATA_SIZE);
  std::fill_n(data.begin(), 'A', DATA_SIZE);
  data.push_back('\r');
  data.push_back('\n');
  std::vector<data_ptr> data_arr(TOTAL);
  for (auto& i: data_arr)
  {
    i = data_ptr(new data_type(data.begin(), data.end()));
  }

  read_buffer buf;
  read_buffer::options_type opt;

  buf.get_options(opt);
  opt.bufsize = 512;
  opt.minbuf = 128;
  opt.maxbuf = 1024;
  opt.sep = "\r\n";
  buf.set_options(opt);

  std::cout << "start... " << std::endl;

  auto start = std::chrono::high_resolution_clock::now();
  long detach_span = 0;
  long init_span = 0;
  for (int i=0; i < TOTAL; ++i)
  {

    auto start1 = std::chrono::high_resolution_clock::now();
    auto beg = data.begin();
    auto end = data.end();
    while ( beg!=end )
    {
      auto p = buf.next();
      auto itr = beg;
      size_t dist = std::distance(beg, end);
      if ( dist > p.second ) { itr += p.second; }
      else { itr = end; }
      std::copy(beg, itr, p.first);
      p.second = std::distance(beg, itr);
      buf.confirm(p);
      beg = itr;
    }
    auto finish1 = std::chrono::high_resolution_clock::now();
    init_span += std::chrono::duration_cast<std::chrono::microseconds>( finish1 - start1).count();

    start1 = std::chrono::high_resolution_clock::now();
    buf.detach();
    finish1 = std::chrono::high_resolution_clock::now();
    detach_span += std::chrono::duration_cast<std::chrono::microseconds>( finish1 - start1).count();

    /*auto d = buf.detach();
    std::cout << i << " " << d->size() << std::endl;
    */
  }
  auto finish = std::chrono::high_resolution_clock::now();
  auto span = std::chrono::duration_cast<std::chrono::microseconds>( finish - start).count();
  
  std::cout << "count: " << TOTAL << std::endl;
  std::cout << "time: " << span << " microseconds" << std::endl;
  std::cout << "rate: " << TOTAL*1000000 / span  << std::endl;
  std::cout << "init time: " << init_span << " microseconds" << std::endl;
  std::cout << "init rate: " << TOTAL*1000000 / init_span  << std::endl;
  std::cout << "detach time: " << detach_span << " microseconds" << std::endl;
  std::cout << "detach rate: " << TOTAL*1000000 / detach_span  << std::endl;
}
