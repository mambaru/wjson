#pragma once


#include <iow/io/stream/ad_initialize.hpp>
#include <iow/io/stream/tags.hpp>

//#include <iow/io/aux/data_pool.hpp>

#include <iow/io/basic/tags.hpp>
#include <fas/aop.hpp>


/// include 
#include <iow/io/reader/stream/aspect.hpp>
#include <iow/io/writer/stream/aspect.hpp>

namespace iow{ namespace io{ namespace stream{
  
template<typename DataType = std::vector<char> >
struct aspect: fas::aspect<
  typename ::iow::io::reader::stream::aspect<DataType>::advice_list,
  typename ::iow::io::writer::stream::aspect<DataType>::advice_list,
  fas::advice< _initialize_, ad_initialize>
>{};

}}}
