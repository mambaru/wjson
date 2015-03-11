#include <fas/aop.hpp>
#include <iow/types.hpp>
#include <iow/descriptor/descriptor_options.hpp>
#include <iow/proactor/pipeline_context.hpp>
#include <iow/proactor/aspect/aspect_pipeline.hpp>
#include <iow/basic/tags.hpp>
#include <iow/aux/memory.hpp>


namespace iow{
  
template<domain D, trasport Tr>
struct aspect_protocol;

template<type Ty>
struct aspect_type;

template<layer L>
struct aspect_layer;

// TODO: убрать

struct ad_make_data
{
  template<typename T>
  auto operator()(T& )
    -> typename T::data_ptr
  {
    return std::make_unique<typename T::data_type>(1024);
  }
};

struct ad_async_read_some
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "ad_async_read_some" << std::endl;
    /*!!!if ( !t.status_() )
      return;
      */
    
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    std::shared_ptr<T> pthis = t.shared_from_this();
    std::cout << "read some...: " << std::string((*dd)->begin(), (*dd)->end()) << std::endl;
    auto callback = [pthis, dd]( boost::system::error_code ec, std::size_t bytes_transferred )
    { 
      (*dd)->resize(bytes_transferred);
      std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
      std::cout << "read some ready...: " << std::string((*dd)->begin(), (*dd)->end()) << std::endl;
      std::cout << ec.message() << std::endl;
      std::cout << bytes_transferred << std::endl;
      
      pthis->get_aspect().template get<_read_handler_>()(*pthis, std::move(*dd) /*, std::move(ec), bytes_transferred*/);
    };
    
    //t.mutex().unlock();
    t.descriptor().async_read_some( ::boost::asio::buffer( **dd ), callback);
    //t.mutex().lock();
  }
};



template<domain D, trasport Tr>
struct aspect_protocol: fas::aspect<
  aspect_pipeline,
  fas::type<_options_type_, descriptor_options>,
  fas::type<_context_type_, pipeline::context< std::vector<char>, std::unique_ptr<std::vector<char>> > >,
  fas::type<_mutex_type_, std::recursive_mutex >,
  fas::type<_descriptor_type_, ::boost::asio::posix::stream_descriptor >,
  fas::advice<_make_data_, ad_make_data>,
  fas::advice<_read_some_, ad_async_read_some>,
  fas::group<_after_start_, _run_line_>
>{};

template<type Ty>
struct aspect_type: fas::aspect<>{};

template<layer L>
struct aspect_layer: fas::aspect<>{};
// END: TODO


template<domain D, trasport Tr, type Ty, layer L, typename A = fas::empty_type>
struct aspect_connection: fas::aspect<
  aspect_protocol<D, Tr>,
  aspect_type<Ty>,
  aspect_layer<L>,
  A
>
{};

}
