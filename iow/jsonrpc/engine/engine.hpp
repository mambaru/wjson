#pragma once
#include <iow/logger/logger.hpp>
#include <iow/jsonrpc/engine/call_map.hpp>
#include <iow/jsonrpc/engine/handler_map.hpp>
#include <iow/jsonrpc/engine/engine_options.hpp>
#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/outgoing/outgoing_holder.hpp>
#include <iow/jsonrpc/types.hpp>
#include <iow/jsonrpc/incoming/aux.hpp>

namespace iow{ namespace jsonrpc{

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

  /***************************************************************/
  /* Управляющие методы                                          */
  /***************************************************************/

  template<typename O>
  void start(O&& opt)
  {
    this->reconfigure( std::forward<O>(opt) );
  }

  template<typename O>
  void reconfigure(O&& opt)
  {
    if ( _common_handler == nullptr )
    {
      auto h = std::make_shared<handler_type>();
      _common_handler = h;
    }

    _handler_factory = [opt, this](io_id_t io_id, outgoing_handler_t handler, bool reg_io) -> handler_ptr
    {
      return this->create_handler_(io_id, opt, std::move(handler), reg_io );
    };
    
    this->upgrate_options_(opt);
    _common_handler->reconfigure(std::forward<O>(opt));
  }

  void stop()
  {
    if ( _common_handler!=nullptr )
    {
      _common_handler->stop();
    }
    _handler_map.clear();
    _call_map.clear();
  }

  template<typename Tg, typename ...Args>
  void call(Args... args)
  {
    _common_handler->template call<Tg>(std::forward<Args>(args)...);
  }

  /***************************************************************/
  /* jsonrpc                                                     */
  /***************************************************************/

  void reg_jsonrpc( io_id_t io_id, outgoing_handler_t handler )
  {
    abort();
    // this->_jsonrpc_factory(io_id, handler, true);
  }

  void perform_jsonrpc(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
  {
    abort();
  }

  /***************************************************************/
  /* data_ptr                                                    */
  /***************************************************************/
  
  outgoing_handler_t io2rpc( ::iow::io::outgoing_handler_t handler )
  {
    return [handler](outgoing_holder holder)
    {
      handler( std::move( holder.detach() ) );
    };
  }

  void reg_io( io_id_t io_id, ::iow::io::outgoing_handler_t handler )
  {
    this->_handler_factory(io_id, io2rpc(handler), true);
  }

  void unreg_io( io_id_t io_id )
  {
    _handler_map.erase(io_id);
  }

  void perform_io(data_ptr d, io_id_t io_id, ::iow::io::outgoing_handler_t handler) 
  {
    using namespace std::placeholders;
    auto rpc = io2rpc( std::move(handler) );
    aux::perform( std::move(d), io_id, std::move( rpc ), std::bind(&engine::perform_incoming, this, _1, _2, _3 ));
  }

  // private: TODO!!!!: переделать на outgoing_jsonrpc_t
// #error TODO!!!!: переделать на outgoing_jsonrpc_t
  void perform_incoming(incoming_holder holder, io_id_t io_id, outgoing_handler_t handler) 
  {
    if ( holder.is_notify() || holder.is_request() )
    {
      this->perform_request_( std::move(holder), io_id, std::move(handler) );
    }
    else if (holder.is_response() || holder.is_error() )
    {
      this->perform_response_( std::move(holder), io_id, std::move(handler) );
    }
    else
    {
      JSONRPC_LOG_ERROR( "jsonrpc::engine: Invalid Request: " << holder.str() )
      aux::send_error( std::move(holder), std::make_unique<invalid_request>(), std::move(handler) );
    }
  }

private:

  template<typename O>
  void upgrate_options_(O& opt)
  {
    typedef typename handler_type::result_handler_t result_handler_t;
    typedef typename handler_type::request_serializer_t request_serializer_t;
    typedef typename handler_type::notify_serializer_t notify_serializer_t;

    if (opt.io_outgoing_handler != nullptr)
    {
      if ( opt.send_request == nullptr )
      {
        auto handler = opt.io_outgoing_handler;
        opt.send_request = [handler, this](const char* name, result_handler_t result_handler, request_serializer_t ser)
        {
          std::cout << "----> engine opt.send_request { "<< std::endl;
          ++this->_call_counter;
          this->_call_map.set(this->_call_counter, std::move(result_handler));
          handler( std::move(ser(name, this->_call_counter)));
          std::cout << "} <----> engine opt.send_request "<< std::endl;
        };
      }

      if ( opt.send_notify == nullptr )
      {
        auto handler = opt.io_outgoing_handler;
        opt.send_notify = [handler, this](const char* name, notify_serializer_t ser)
        {
          handler( std::move(ser(name)) );
        };
      }
    }
  }


  void perform_request_(incoming_holder holder, io_id_t io_id, outgoing_handler_t outgoing_handler) 
  {
    auto handler = _direct_mode ? _common_handler : _handler_factory(io_id, outgoing_handler, false);
    handler->invoke( std::move(holder), std::move(outgoing_handler) );
  }

  void perform_response_(incoming_holder holder, io_id_t /*io_id*/, outgoing_handler_t handler) 
  {
    call_id_t call_id = holder.get_id<call_id_t>();
    if ( auto result_handler = _call_map.detach(call_id) )
    {
      result_handler(std::move(holder));
    }
    else if ( handler!=nullptr )
    {
      JSONRPC_LOG_WARNING("jsonrpc::engind incoming response with call_id=" << call_id << " not found")
      handler( outgoing_holder() );
    }
  }

  template<typename Opt>
  handler_ptr create_handler_(io_id_t io_id, Opt opt, outgoing_handler_t handler, bool reg_io)
  {
    bool reinit;
    auto ph = _handler_map.findocre(io_id, reg_io, reinit);
    if ( reinit )
    {
      opt.io_outgoing_handler = std::move(handler);
      this->upgrate_options_(opt);
      if ( !ph->status() )
      {
        ph->start(opt);
      }
      else if ( reinit )
      {
        ph->reconfigure(opt);
      }
    }
    return ph;
  }

private:

  std::function<handler_ptr(io_id_t, outgoing_handler_t, bool)> _handler_factory;

  typedef handler_map<handler_type> handler_map_t;
  handler_map_t _handler_map;
  handler_ptr _common_handler;
  call_map _call_map;
  std::atomic<int> _call_counter;
  std::atomic<bool> _direct_mode;
};

}}
