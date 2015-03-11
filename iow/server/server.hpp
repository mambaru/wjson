#include <iow/connection/connection_base.hpp>
#include <memory>

namespace iow{
  
template<domain D, trasport Tr, type Ty, layer L, typename A = fas::empty_type>
class connection
  : public connection_base< aspect_connection<D, Tr, Ty, L, A> >
{
  typedef connection<D, Tr, Ty, L, A> self;
  typedef connection_base< aspect_connection<D, Tr, Ty, L, A> > super;
  
  typedef typename super::data_ptr data_ptr;
  typedef typename super::mutex_type mutex_type;
  
public:
  
  template<typename T>
  void write(T& t, data_ptr d)
  {
    std::lock_guard<mutex_type> lk(super::mutex());
    super::write_(*this, std::move(d) );
  }
  
public:
  
};
