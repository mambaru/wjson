#pragma once

struct s;
namespace iow{ namespace jsonrpc{
  
template<typename HandlerOptions>
struct engine_options: HandlerOptions
{
  typedef HandlerOptions handler_options;
  bool allow_callback=true;
};

template<typename JsonrpcHandler>
class engine
  : public JsonrpcHandler
{
public:
  typedef JsonrpcHandler super;
  typedef JsonrpcHandler handler_type;
  typedef typename handler_type::options_type handler_options_type;
  typedef engine_options<handler_options_type> options_type;
  
  typedef typename super::holder_type holder_type;
  typedef typename super::io_id_t io_id_t;
  typedef typename super::outgoing_handler_t outgoing_handler_t;
  typedef typename super::data_ptr data_ptr;

  template<typename O>
  void start(O&& opt)
  {
    std::lock_guard< typename super::mutex_type > lk( super::mutex() );
    _direct_mode = !opt.allow_callback;
    super::start_(*this, std::forward<O>(opt));
  }
  
  template<typename O>
  void initialize(O&& opt)
  {
    std::lock_guard< typename super::mutex_type > lk( super::mutex() );
    _direct_mode = !opt.allow_callback;
    super::initialize_(*this, std::forward<O>(opt));
  }
  
  void stop()
  {
    std::lock_guard< typename super::mutex_type > lk( super::mutex() );
    super::stop_(*this);
  }

  void invoke(holder_type holder, io_id_t io_id, outgoing_handler_t outgoing_handler) 
  {
    if ( _direct_mode )
    {
      super::invoke( std::move(holder), std::move(outgoing_handler) );
    }
    else
    {
    }
  }
  
  void invoke(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
  {
    incoming_holder holder( std::move(d) );
    holder.parse();
    this->invoke( std::move(holder), io_id, std::move(handler) );
  }

private:
  
  std::atomic<bool> _direct_mode;
};


}}
