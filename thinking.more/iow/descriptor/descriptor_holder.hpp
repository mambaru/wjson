#pragma once

#include <iow/basic/io_base.hpp>
#include <fas/typemanip/has_typename.hpp>

namespace iow{
  
FAS_HAS_TYPENAME(has_protocol_type, protocol_type)

template<typename A = fas::aspect<> >
class descriptor_holder
  : public io_base< A >
{
public:
  
  typedef descriptor_holder<A> self;
  typedef io_base< A > super;
  
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::context_type context_type;
  typedef typename context_type::data_type data_type;
  typedef typename context_type::data_ptr  data_ptr;
  
  typedef typename context_type::outgoing_handler_t outgoing_handler_t;
  typedef typename context_type::incoming_handler_t incoming_handler_t;
  typedef typename context_type::startup_handler_t  startup_handler_t;
  typedef typename context_type::shutdown_handler_t shutdown_handler_t;

  typedef typename super::aspect::template advice_cast<_descriptor_type_>::type descriptor_type;
  
  descriptor_holder(io_service_type& io, const options_type& opt)
    : super( io, opt)
    , _descriptor( io )
  {}

  /*
  descriptor_holder(const options_type& opt)
    : super( opt)
    , _descriptor( super::get_io_service() )
  {}*/
  
  /*
   * 1. создаем новый объект
   * 2. dub native_handle 
   * 3. делаем assign сновым дескриптором
   */
  
  /*
  typename descriptor_type::native_handle_type 
  dup_native_() const
  {
    return ::dup( this->descriptor().native_handle() );
  }
  
  template<typename DescriptorType, typename ProtocolType>
  DescriptorType&& dup_descriptor_(io_service_type& io, const ProtocolType& protocol)
  {
    return std::move( DescriptorType(io, protocol, this->dup_native_() ) );
  }

  template<typename DescriptorType>
  DescriptorType&& dup_descriptor_helper_(io_service_type& io, fas::false_)
  {
    return std::move( DescriptorType(io, this->dup_native_() ) );
  }

  template<typename DescriptorType>
  DescriptorType&& dup_descriptor_helper_(io_service_type& io, fas::true_)
  {
    return std::move( this->dup_descriptor_(io, this->descriptor().local_endpoint().protocol() ) );
  }
  
  template<typename DescriptorType>
  DescriptorType&& dup_descriptor_(io_service_type& io)
  {
    return std::move(
      this->dup_descriptor_helper_<DescriptorType>(
        io,
        fas::bool_< has_protocol_type<DescriptorType>::value >()
      )
    );
  }

  template<typename Holder, typename ProtocolType>
  Holder&& dup_holder_(io_service_type& io, const ProtocolType& protocol, const typename Holder::options_type& opt)
  {
    return std::move(Holder( std::move(this->dup_descriptor_< typename Holder::descriptor_type >(io, protocol)), opt));
  }

  self&& dup_holder_(io_service_type& io, const options_type& opt)
  {
    return std::move(self( std::move(this->dup_descriptor_< descriptor_type >(io)), opt));
  }

  self&& dup_holder_(const options_type& opt, fas::false_ )
  {
    return std::move( self::dup_holder_(super::get_io_service(), opt) );
  }

  self&& dup_holder_(const options_type& opt, fas::false_ )
  {
    return std::move( self::dup_holder_(super::get_io_service(), opt) );
  }
  */

  /*
  self&& dup_holder_()
  {
    return std::move( this->dup_holder_( this->options_() ) );
  }
  */

  
  
  /*
  template<typename Holder>
  Holder dup(typename Holder::options_type& opt)
  {
    typedef typename Holder::descriptor_type dup_descriptor_type;
    typedef typename dup_descriptor_type::native_handle_type dup_native_type;
    dup_native_type f = ::dup( this->descriptor().native_handle() );
    dup_descriptor_type dup_descriptor(this->get_io_service(), typename descriptor_type::protocol_type(), f );
    return Holder( std::move(dup_descriptor), opt );
  }
  */
  
  descriptor_type& descriptor()
  {
    return _descriptor;
  }

  const descriptor_type& descriptor() const
  {
    return _descriptor;
  }

// ------------------------------

  descriptor_type& descriptor_()
  {
    return _descriptor;
  }

  const descriptor_type& descriptor_() const
  {
    return _descriptor;
  }


protected:
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
  
  descriptor_type _descriptor;
};

}
