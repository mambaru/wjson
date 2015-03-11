#pragma once

#include <iow/proactor/pipeline.hpp>
#include <iow/proactor/ioline/aspect/aspect_ioline.hpp>

namespace iow{
  
  /*
template<typename A = fas::empty_type >
class ioline
  : public pipeline< typename fas::merge_aspect<A, aspect_ioline>::type  >
{
public:
  
  typedef ioline<A> self;
  typedef pipeline< typename fas::merge_aspect<A, aspect_ioline>::type > super;
  
  typedef typename super::options_type options_type;
  typedef typename super::io_service_type io_service_type;

  ioline(io_service_type& io, const options_type& opt)
    : super( io, opt)
  {
  }

protected:

private:

};
*/

}
