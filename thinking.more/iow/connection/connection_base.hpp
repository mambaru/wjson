#include <iow/descriptor/descriptor_holder.hpp>
#include <iow/connection/tags.hpp>
#include <memory>

namespace iow{
  
template<typename A = fas::aspect<> >
class connection_base
  : public descriptor_holder<A>
{
  typedef connection_base<A> self;
  typedef descriptor_holder<A> super;
public:
  typedef typename super::data_ptr data_ptr;
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;
  
  connection_base(io_service_type& io, const options_type& opt)
    : super( io, opt)
  {}

public:
  template<typename T>
  void write_(T& t, data_ptr d)
  {
    t.get_aspect().template get<_write_>()(t, std::move(d) );
  }
};

}