#include <iow/descriptor/descriptor_holder.hpp>
#include <iow/server/tags.hpp>
#include <memory>

namespace iow{
  
template<typename A = fas::aspect<> >
class server_base
  : public descriptor_holder<A>
{
  typedef connection_base<A> self;
  typedef descriptor_holder<A> super;
    
public:
public:
  
};
