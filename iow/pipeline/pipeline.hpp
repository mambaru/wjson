#pragma once

#include <iow/descriptor/descriptor_holder.hpp>
#include <iow/pipeline/aspect/aspect_pipeline.hpp>
#include <fas/typemanip/has_typename.hpp>

namespace iow{
  
//FAS_HAS_TYPENAME(has_protocol_type, protocol_type)

template<typename A = fas::empty_type >
class pipeline
  : public descriptor_holder< typename fas::merge_aspect<A, aspect_pipeline>::type  >
{
public:
  
  typedef pipeline<A> self;
  typedef descriptor_holder< typename fas::merge_aspect<A, aspect_pipeline>::type > super;
  
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::aspect::template advice_cast<_descriptor_type_>::type descriptor_type;
  
  pipeline(io_service_type& io, const options_type& opt)
    : super( io, opt)
  {
  }
  
  /*
  typename descriptor_type::native_handle_type 
  dup_native_() const
  {
    return ::dup( this->descriptor().native_handle() );
  }
  
  template<typename DescriptorType, typename IOServiceType, typename ProtocolType>
  DescriptorType dup_descriptor_(IOServiceType& io, const ProtocolType& protocol)
  {
    return std::move( DescriptorType(io, protocol, this->dup_native_() ) );
  }

  template<typename DescriptorType, typename IOServiceType>
  DescriptorType dup_descriptor_1(IOServiceType& io, fas::false_)
  {
    return std::move( DescriptorType(io, this->dup_native_() ) );
  }

  template<typename DescriptorType, typename IOServiceType>
  DescriptorType dup_descriptor_1(IOServiceType& io, fas::true_)
  {
    typedef DescriptorType dup_descriptor_type;
    typedef typename dup_descriptor_type::native_handle_type dup_native_type;
    dup_native_type f = ::dup( this->descriptor().native_handle() );
    dup_descriptor_type dup_descriptor(io, this->descriptor().local_endpoint().protocol(), f);
    return std::move(dup_descriptor);
  }
  
  template<typename DescriptorType, typename IOServiceType>
  DescriptorType dup_descriptor_(IOServiceType& io)
  {
    return std::move(
      this->dup_descriptor_1<DescriptorType>(
        io,
        fas::bool_< has_protocol_type<DescriptorType>::value >()
      )
    );
  }

  template<typename Holder, typename ProtocolType>
  Holder dup_holder_(io_service_type& io, const ProtocolType& protocol, const typename Holder::options_type& opt)
  {
    return std::move(Holder( std::move(this->dup_descriptor_< typename Holder::descriptor_type >(io, protocol)), opt));
  }

  self&& dup_holder_(io_service_type& io, const options_type& opt)
  {
    return std::move(self( std::move(this->dup_descriptor_< descriptor_type >(io)), opt));
  }

  self&& dup_holder_(const options_type& opt)
  {
    return std::move( this->dup_holder_( this->get_io_service(), opt) );
  }

  self&& dup_holder_()
  {
    return std::move( this->dup_holder_( this->options_() ) );
  }

  descriptor_type& descriptor()
  {
    return _descriptor;
  }

  const descriptor_type& descriptor() const
  {
    return _descriptor;
  }
  */
  

protected:
  /*
  template<typename T>
  void stop_(T& t)
  {
    if ( this->descriptor().is_open() )
    {
      boost::system::error_code ec;
      t.descriptor().cancel(ec);
      if ( !ec )
        this->descriptor().close(ec);
    }
    
    super::stop_(t);
  }
  */
  
  /*
  template<typename DescriptorType, typename IOService, typename ProtocolType>
  DescriptorType&& clone(IOService& io, const ProtocolType& protocol)
  {
    return DescriptorType( this->dup<DescriptorType>(io, protocol),  options_type() );
  }

  template<typename DescriptorType>
  std::shared_ptr<DescriptorType> clone( const options_type& opt)
  {
    return std::make_shared< DescriptorType >( this->dup<descriptor_type>(io, protocol),  opt );
  }
  */

private:
  //descriptor_type _descriptor;
};

}
