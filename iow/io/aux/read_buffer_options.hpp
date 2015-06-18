#pragma once

#include <string>
#include <memory>
#include <functional>

namespace iow{ namespace io{
  
template<typename DataType>
struct read_buffer_options
{
  typedef std::unique_ptr<DataType> data_ptr;
  typedef std::function< data_ptr(size_t, size_t) > create_fun;
  typedef std::function< void(data_ptr) > free_fun;

  std::string sep;
  size_t bufsize=4096;
  size_t maxbuf=4096*2;
  size_t minbuf=0;
  bool fast_mode = false;
  bool trimsep = false; // Отрезать сепаратор 
  std::function< data_ptr(size_t, size_t) > create;
  std::function< void(data_ptr) > free;
};

}}
