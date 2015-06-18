#pragma once
#include <iow/logger/logger.hpp>

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
      abort();
    }
  }
  
  void invoke(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
  {
    try
    {
      while (d != nullptr)
      {
        incoming_holder holder(std::move(d));
        d = holder.parse();
        if ( holder.is_notify() || holder.is_request() )
        {
          this->invoke( std::move(holder), io_id, std::move(handler) );
        }
        else
        {
          if ( handler!=nullptr )
          {
            handler(nullptr);
          }
        }
          
        //t.get_aspect().template get<_verify_>()( t,  std::move(hold), io_id, outgoing_handler );
      }
    }
    catch(const json::json_error& er)
    {
      JSONRPC_LOG_WARNING( "jsonrpc::service parse error: " << er.what() )
      //! t.get_aspect().template get<_callback_error_>()(t, parse_error(), outgoing_handler );
    }
    catch(const std::exception& ex)
    {
      JSONRPC_LOG_ERROR( "jsonrpc::service parse error: " << ex.what() )
      //! t.get_aspect().template get<_callback_error_>()(t, server_error(ex.what() ), outgoing_handler);
    }
    catch(...)
    {
      JSONRPC_LOG_ERROR( "jsonrpc::service server error: " << "unhandler exception" )
      //! t.get_aspect().template get<_callback_error_>()(t, server_error(), outgoing_handler);
    }
    
    /*
    incoming_holder holder( std::move(d) );
    holder.parse();
    this->invoke( std::move(holder), io_id, std::move(handler) );
    */
  }

private:
  
  std::atomic<bool> _direct_mode;
};


}}
