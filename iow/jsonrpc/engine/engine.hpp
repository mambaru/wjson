#pragma once
#include <iow/logger/logger.hpp>
#include <iow/jsonrpc/engine/call_registry.hpp>

namespace iow{ namespace jsonrpc{
  
template<typename HandlerOptions>
struct engine_options: HandlerOptions
{
  typedef HandlerOptions handler_options;
  bool allow_callback=true;
  ::iow::io::outgoing_handler_t outgoing_handler1;
};

template<typename JsonrpcHandler>
class engine
{
public:
  typedef JsonrpcHandler handler_type;
  typedef typename handler_type::target_type target_type;
  typedef std::shared_ptr<handler_type> handler_ptr;
  typedef typename handler_type::options_type handler_options_type;
  
  typedef engine_options<handler_options_type> options_type;
  
  typedef typename handler_type::io_id_t io_id_t;
  typedef typename handler_type::outgoing_handler_t outgoing_handler_t;
  typedef typename handler_type::data_ptr data_ptr;
  typedef typename outgoing_holder::call_id_t call_id_t;
  
  template<typename O>
  void start(O&& opt)
  {
    //std::lock_guard< typename super::mutex_type > lk( super::mutex() );
    _handler = std::make_shared<handler_type>();
    this->upgrate_options_(opt);
    initialize_(opt);
    _handler->start( std::forward<O>(opt));
  }
  
  template<typename O>
  void reconfigure(O&& opt)
  {
    //std::lock_guard< typename super::mutex_type > lk( super::mutex() );
    this->upgrate_options_(opt);
    initialize_(opt);
    _handler->reconfigure(std::forward<O>(opt));
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
      
      // Если задан outgoing_handler то сериализуем и отправляем полностью
      // Если задан holder_handler то создаем outgoing_holder
      if (opt.outgoing_handler1 != nullptr)
      {
        auto handler = opt.outgoing_handler1;
        opt.send_request = [handler, this](const char* name, result_handler_t result_handler, request_serializer_t ser)
        {
          ++this->_call_counter;
          this->_call_registry.set(this->_call_counter, std::move(result_handler));
          handler( std::move(ser(name, this->_call_counter)));
        };
        
        opt.send_notify = [handler, this](const char* name, notify_serializer_t ser)
        {
          handler( std::move(ser(name)) );
        };

      }
      else
      {
        IOW_LOG_FATAL("iow::jsonrpc::engine outgoing_handler==nullptr")
        // abort();

      }

      /*
      opt.send_request = [](const char* name, result_handler_t handler, request_serializer_t ser)
      {
        // outgoing_holder(const char* name, data_ptr d, result_handler_t result_handler, time_point_t  time_point = time_point_t())
        // outgoing_holder(const char* name, request_serializer_t serializer, result_handler_t result_handler, time_point_t time_point = time_point_t())
        
        outgoing_holder oh(name, [name,ser](typename handler_type::call_id_t id)
        {
          return std::move(ser(name, id));
        },
          handler
        );
        IOW_LOG_FATAL("ku-ku request")
        abort();
      };
      opt.send_notify = [](const char* name, notify_serializer_t){
        IOW_LOG_FATAL("ku-ku notify")
        abort();
      };
      */
  }


  void stop()
  {
    //std::lock_guard< typename super::mutex_type > lk( super::mutex() );
    _handler->stop();
  }

  void invoke(incoming_holder holder, io_id_t /*io_id*/, outgoing_handler_t outgoing_handler) 
  {
    // TODO: mutex??
    if ( _direct_mode )
    {
      // не нужно регистрировать коннект, работает быстрее
      _handler->invoke( std::move(holder), std::move(outgoing_handler) );
    }
    else
    {
      JSONRPC_LOG_FATAL( "jsonrpc::engine: реализован только _direct_mode=true(allow_callback=false)" )
      abort();
    }
  }

  void perform(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
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
      // Ахтунг! сделать в кретической секции и в нвндлере
      _handler->get_aspect().template get<_invoke_error_>()
        ( *_handler, std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(handler));
      return;
    }
    catch(...)
    {
      JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << "unhandler exception" )
      _handler->get_aspect().template get<_invoke_error_>()
        ( *_handler, std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(handler));
      return;
    }
  }
  
  template<typename Tg, typename ...Args>
  void call(Args... args)
  {
    _handler->template call<Tg>(std::forward<Args>(args)...);
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
      _handler->get_aspect().template get<_invoke_error_>()
        (*_handler, std::move(holder), std::make_unique<parse_error>(), std::move(handler));
      return nullptr;
    }

    if ( holder.is_notify() || holder.is_request() )
    {
      this->invoke( std::move(holder), io_id, std::move(handler) );
    }
    else if (holder.is_response() )
    {
      call_id_t call_id = holder.get_id<call_id_t>();
      auto result_handler = _call_registry.detach(call_id);
      result_handler(std::move(holder));
      /*JSONRPC_LOG_FATAL("engine::invoke_once_ result message not impl")
      abort();
      */
    }
    else if (holder.is_error() )
    {
      JSONRPC_LOG_FATAL("result ERROR message not impl")
      abort();
    }
    else
    {
      JSONRPC_LOG_WARNING( "jsonrpc::engine: Invalid Request: " << holder.str() )
      _handler->get_aspect().template get<_invoke_error_>()
        (*_handler, std::move(holder), std::make_unique<invalid_request>(), std::move(handler));
    }
    return std::move(d);
  }

private:
  handler_ptr _handler;
  call_registry _call_registry;
  std::atomic<int> _call_counter;
  std::atomic<bool> _direct_mode;
};


}}
