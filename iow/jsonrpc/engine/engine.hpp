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
  typedef std::shared_ptr<handler_type> handler_ptr;
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
    this->upgrate_options_(opt);
    initialize_(opt);
    super::start_(*this, std::forward<O>(opt));
  }
  
  template<typename O>
  void reconfigure(O&& opt)
  {
    std::lock_guard< typename super::mutex_type > lk( super::mutex() );
    this->upgrate_options_(opt);
    initialize_(opt);
    super::reconfigure_(*this, std::forward<O>(opt));
  }

  template<typename O>
  void initialize_(O opt)
  {
    _direct_mode = !opt.allow_callback;
    create_handler_ = [opt](outgoing_handler_t /*handler*/) mutable -> handler_ptr 
    {
      auto ph = std::make_shared<handler_type>();
      ph->start(opt);
      return ph;
    };
  }
  
  template<typename O>
  void upgrate_options_(O& opt)
  {
      typedef typename handler_type::result_handler_t result_handler_t;
      typedef typename handler_type::request_serializer_t request_serializer_t;
      typedef typename handler_type::notify_serializer_t notify_serializer_t;
      opt.send_request = [](const char* /*name*/, result_handler_t, request_serializer_t)
      {
        //outgoing_holder(const char* name, data_ptr d, result_handler_t result_handler, time_point_t  time_point = time_point_t())
        //outgoing_holder()
        IOW_LOG_FATAL("ku-ku request")
        abort();
      };
      opt.send_notify = [](const char* /*name*/, notify_serializer_t){
        IOW_LOG_FATAL("ku-ku notify")
        abort();
      };
  }


  void stop()
  {
    std::lock_guard< typename super::mutex_type > lk( super::mutex() );
    super::stop_(*this);
  }

  void invoke(holder_type holder, io_id_t /*io_id*/, outgoing_handler_t outgoing_handler) 
  {
    // TODO: mutex??
    if ( _direct_mode )
    {
      // не нужно регистрировать коннект, работает быстрее
      super::invoke( std::move(holder), std::move(outgoing_handler) );
    }
    else
    {
      JSONRPC_LOG_FATAL( "jsonrpc::engine: реализован только _direct_mode=true(allow_callback=false)" )
      abort();
    }
  }

  void invoke(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
  {
    try
    {
      while (d != nullptr)
      {
        d = this->invoke_once_(std::move(d), io_id, handler);
      }
    }
    catch(const std::exception& ex)
    {
      JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << ex.what() )
      super::get_aspect().template get<_invoke_error_>()
        (*this, std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(handler));
      return;
    }
    catch(...)
    {
      JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << "unhandler exception" )
      super::get_aspect().template get<_invoke_error_>()
        (*this, std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(handler));
      return;
    }
  }
  
  template<typename Tg, typename ...Args>
  void call(Args... args)
  {
    super::template call<Tg>(std::forward<Args>(args)...);
  }

  
  void reg_io( io_id_t io_id, outgoing_handler_t handler )
  {
    JSONRPC_LOG_MESSAGE( "jsonrpc::engine: reg_io" )
  }
  
  void unreg_io( io_id_t io_id )
  {
    JSONRPC_LOG_MESSAGE( "jsonrpc::engine: unreg_io" )
  }
  

private:
  
  std::function<handler_ptr(outgoing_handler_t handler)> create_handler_;

  data_ptr invoke_once_(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
  {
    incoming_holder holder(std::move(d));
    try
    {
      d = holder.parse();
    }
    catch(const json::json_error& er)
    {
      JSONRPC_LOG_WARNING( "jsonrpc::engine: parse error: " << holder.error_message(er) )
      super::get_aspect().template get<_invoke_error_>()
        (*this, std::move(holder), std::make_unique<parse_error>(), std::move(handler));
      return nullptr;
    }

    if ( holder.is_notify() || holder.is_request() )
    {
      this->invoke( std::move(holder), io_id, std::move(handler) );
    }
    else if (holder.is_response() )
    {
      JSONRPC_LOG_FATAL("result message not impl")
      abort();
    }
    else if (holder.is_error() )
    {
      JSONRPC_LOG_FATAL("result ERROR message not impl")
      abort();
    }
    else
    {
      JSONRPC_LOG_WARNING( "jsonrpc::engine: Invalid Request: " << holder.str() )
      super::get_aspect().template get<_invoke_error_>()
        (*this, std::move(holder), std::make_unique<invalid_request>(), std::move(handler));
    }
    return std::move(d);
  }

private:
  std::atomic<bool> _direct_mode;
};


}}
