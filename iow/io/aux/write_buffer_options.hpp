#pragma once

#include <iow/memory.hpp>
#include <queue>
#include <memory>
#include <string>
#include <iostream>

namespace iow{ namespace io{

template<typename DataType>
struct write_buffer_options
{
  typedef std::unique_ptr<DataType> data_ptr;
  typedef std::function< data_ptr(size_t, size_t) > create_fun;
  typedef std::function< void(data_ptr) > free_fun;
  
  std::string sep = std::string("\r\n");
  size_t bufsize = 8*1024;
  size_t maxbuf  = 8*1024; 
  size_t minbuf  = 0; 
  bool first_as_is = true; // Если maxbuff или minbuff != 0 и bufsize!=0
  
  create_fun create;
  free_fun free;
};

}}
