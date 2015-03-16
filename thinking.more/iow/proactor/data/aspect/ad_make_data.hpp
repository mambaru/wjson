#pragma once


namespace iow{ 
  
struct ad_make_incoming_data
{
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    return t.context_().incoming_data.create();
    //return std::make_unique<typename T::data_type>( t.options().input_buffer_size );
  }
};


struct ad_prepare_data
{
  template<typename T>
  typename T::data_ptr operator()(T& t, typename T::data_ptr d)
  {
    return t.context_().incoming_buffer.create();
  }
};

}