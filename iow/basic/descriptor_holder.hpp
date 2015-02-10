#pragma once

#include <wfc/io/tags.hpp>
#include <wfc/io/basic_io.hpp>
#include <fas/aop.hpp>
#include <functional>
#include <boost/asio.hpp>
namespace wfc{ namespace io{ 

struct _holder_create_;

struct ad_holder_create
{
  template<typename T/*, typename Tmp*/>
  void operator()(T& t/*, Tmp*/)
  {
    t.get_aspect().template get<_status_>()=true;
  }
};

/*
struct ad_error_code
{
  typedef ::boost::system::error_code error_code;
private:
  value
}
*/
  
struct holder_aspect: fas::aspect<
  fas::advice< _holder_create_, ad_holder_create>,
  fas::group< _on_create_, _holder_create_>,
  fas::value< _status_, bool>/*,
  fas::value< _error_code_, boost::system::error_code> // todo; отдельно для reader, writer и про
  */
>{};

  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class descriptor_holder
  : public basic_io< typename fas::merge_aspect<A, holder_aspect>::type , AspectClass>
  // , std::enable_shared_from_this< descriptor_holder<A, AspectClass> >
{
public:
  
  typedef descriptor_holder<A, AspectClass> self;
  typedef basic_io< typename fas::merge_aspect<A, holder_aspect>::type , AspectClass> super;
  typedef typename super::aspect::template advice_cast<_descriptor_type_>::type descriptor_type;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  

  descriptor_holder(descriptor_type&& desc, const options_type& conf/*, ::wfc::io::incoming_handler handler = nullptr*/)
    : super( desc.get_io_service(), conf)
    , _descriptor( std::move(desc) )
    // , _handler(handler)
    //, _handler(conf.incoming_handler)
  {
  }
  
  ~descriptor_holder()
  {
    
  }

  template<typename Descriptor, typename IOServiceType, typename ProtocolType>
  Descriptor dup(IOServiceType& io, const ProtocolType& protocol)
  {
    typedef Descriptor dup_descriptor_type;
    typedef typename dup_descriptor_type::native_handle_type dup_native_type;
    dup_native_type f = ::dup( this->descriptor().native_handle() );
    dup_descriptor_type dup_descriptor(io, protocol, f);
    return std::move(dup_descriptor);
  }

  
  
  template<typename Holder>
  Holder dup(typename Holder::options_type& opt)
  {
    typedef typename Holder::descriptor_type dup_descriptor_type;
    typedef typename dup_descriptor_type::native_handle_type dup_native_type;
    dup_native_type f = ::dup( this->descriptor().native_handle() );
    dup_descriptor_type dup_descriptor(this->get_io_service(), typename descriptor_type::protocol_type(), f );
    return Holder( std::move(dup_descriptor), opt );
  }
  
  
  
  descriptor_type& descriptor()
  {
    return _descriptor;
  }

  const descriptor_type& descriptor() const
  {
    return _descriptor;
  }
  
  bool status() const 
  {
    return this->get_aspect().template get< ::wfc::io::_status_>();
  }
  
  /*
  const boost::system::error_code& error_code() const 
  {
    return this->get_aspect().template get< ::wfc::io::_error_code_>();
  }
  */

  template<typename T>
  void self_stop(T& t, std::function<void()> finalize)
  {
    if ( this->descriptor().is_open() )
    {
      boost::system::error_code ec;
      this->descriptor().close(ec);
    }
    
    super::self_stop(t, finalize);
  }
  
  ::wfc::callback_owner _tmp;

  template<typename T>
  void stop_impl(T& t, std::function<void()> finalize)
  {
      if ( t.descriptor().is_open() )
      {
        boost::system::error_code ec;
        t.descriptor().cancel(ec);
        if ( !ec )
          t.descriptor().close(ec);
      }
    
      super::stop(t, finalize);
  }
  template<typename T>
  void stop(T& t, std::function<void()> finalize)
  {
    //typename super::lock_guard lk( super::mutex() );
    stop_impl(t, finalize);
    return;
    
    
    std::atomic<bool> flag(false);
    
    auto pthis = t.shared_from_this();
    //std::weak_ptr<typename decltype(pthis)::element_type> wthis(pthis);
    super::get_io_service().post( super::super_wrap( [pthis, &t, &flag, finalize]()
    {
      pthis->stop_impl(t, finalize);
      flag = true;
    },
    [&flag](){ flag=true;}
    ));
    /*
    super::get_io_service().post( super::strand().wrap( [wthis, &t, &flag, finalize]()
    {
      if ( auto pthis = wthis.lock() )
        pthis->stop_impl(t, finalize);
      flag = true;
    }));
    */
    
    
    while (!flag)
    {
      super::get_io_service().reset();
      super::get_io_service().poll();
    }
  }

  
  template<typename AcceptorType, typename IOService, typename ProtocolType>
  std::shared_ptr<AcceptorType> clone(IOService& io, const ProtocolType& protocol)
  {
    return std::make_shared< AcceptorType >( this->dup<descriptor_type>(io, protocol),  super::options() );
  }

private:
  
  descriptor_type _descriptor;

public:
  // TODO: в аспект
  //::wfc::io::incoming_handler_t _handler;
  

};

}}
