
#include <iostream>
#include <sstream>
#include <memory>
#include <iow/basic/descriptor_holder.hpp>
#include <iow/aux/memory.hpp>
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
  fas::type< ::iow::_descriptor_type_, ::boost::asio::ip::tcp::socket>,
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
    : super( io, opt )
  {
  }
};


template<typename T, typename S>
class stat_wrapper: public T
{
  typedef T super;
public:
  stat_wrapper(T t,  S stat)
    : T(std::move(t))
    , _stat(std::move(stat))
  {}
private:
  S _stat;
};


template<typename T>
class default_delete
{
public:
  default_delete()
  {
    std::cout << "default_delete()" << std::endl;
  }

  ~default_delete()
  {
    std::cout << "~default_delete()" << std::endl;
  }

  default_delete(default_delete&&)
  {
    std::cout << "default_delete(default_delete&&)" << std::endl;
  }

  void operator()(T* t)
  {
    std::cout << "~default_delete::operator()()" << std::endl;
    delete t;
  }
};

int main()
{
  /*
  {
    auto x = std::make_unique<int >(10);
    std::cout << *x << std::endl;
    //std::get_deleter<default_delete<int>>(x);
  }
  return 0;
  auto tst = stat_wrapper< std::unique_ptr<int>, fas::empty_type>(nullptr, fas::empty_type());
  */
  //*tst = 10;
  descriptor_holder::io_service_type io_service;
  descriptor_holder holder(io_service, fas::empty_type() );
  
  //holder.dup_holder_();
  
  return 0;
}
