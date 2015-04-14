#pragma once

namespace iow{ namespace io{ namespace descriptor{

template<typename ContextType>
struct options
{
private:
  typedef ContextType context_type;
public:
  typedef typename context_type::data_type data_type;
  typedef typename context_type::data_ptr  data_ptr; 
  typedef typename context_type::outgoing_handler_t  outgoing_handler_t;
  typedef typename context_type::incoming_handler_t  incoming_handler_t;
  typedef typename context_type::startup_handler_t   startup_handler_t;
  typedef typename context_type::shutdown_handler_t  shutdown_handler_t;

  outgoing_handler_t outgoing_handler;
  incoming_handler_t incoming_handler;
  startup_handler_t  startup_handler;
  shutdown_handler_t shutdown_handler;
};
  
}}}
