#include <iow/connection/connection_base.hpp>
#include <iow/connection/aspect_connection.hpp>
#include <iow/types.hpp>
#include <memory>

namespace iow{
  
template<domain D, trasport Tr, type Ty, layer L, typename A = fas::empty_type>
class connection
  : public connection_base< aspect_connection<D, Tr, Ty, L, A> >
  , public std::enable_shared_from_this< connection<D, Tr, Ty, L, A> >
{
  typedef connection<D, Tr, Ty, L, A> self;
  typedef connection_base< aspect_connection<D, Tr, Ty, L, A> > super;

public:  
  typedef typename super::data_ptr data_ptr;
  typedef typename super::mutex_type mutex_type;
  
  IOW_INHERITING_CONSTRUCTORS_T(connection, super)
  IOW_IO_BASE_IMPL_T(super)

  
  template<typename T>
  void write(T& t, data_ptr d)
  {
    std::lock_guard<mutex_type> lk(super::mutex());
    super::write_(*this, std::move(d) );
  }
  
public:
  
};

}