
#include <iostream>
#include <sstream>

#include <iow/basic/descriptor_holder.hpp>
#include <boost/asio.hpp>

/*
typedef fas::aspect< fas::type_list_n<
  fas::type< ::iow::_descriptor_type_, ::boost::asio::posix::stream_descriptor>,
  fas::type< ::iow::_options_type_, fas::empty_type>,
  fas::type< ::iow::_mutex_type_, std::mutex>
  >::type
> aspectx;
*/

struct aspect_test: fas::aspect< fas::type_list_n<
  // fas::type< ::iow::_descriptor_type_, ::boost::asio::posix::stream_descriptor>,
  // fas::type< ::iow::_descriptor_type_, ::boost::asio::ip::tcp::socket>,
  fas::type< ::iow::_options_type_, fas::empty_type>,
  fas::type< ::iow::_mutex_type_, std::mutex>
  >::type
> {};

class descriptor_holder
  : public ::iow::descriptor_holder<aspect_test>
{
public:
  typedef ::iow::descriptor_holder<aspect_test> super;
  typedef super::io_service_type io_service_type;
  typedef super::options_type options_type;
  typedef super::descriptor_type descriptor_type;
  
  descriptor_holder(io_service_type& io, options_type opt)
    : super( std::move(descriptor_type(io)), opt )
  {
  }
};

int main()
{
  descriptor_holder::io_service_type io_service;
  descriptor_holder holder(io_service, fas::empty_type() );
  
  holder.dup_holder_();
  
  return 0;
}
