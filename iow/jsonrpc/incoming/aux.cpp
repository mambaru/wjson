#include <iow/jsonrpc/incoming/aux.hpp>
#include <iow/jsonrpc/incoming/incoming_holder.hpp>
#include <iow/jsonrpc/outgoing/outgoing_error_json.hpp>
#include <iow/jsonrpc/errors/error_json.hpp>
#include <iow/jsonrpc/types.hpp>
#include <iow/logger/logger.hpp>

namespace iow{ namespace jsonrpc{ namespace aux{

  namespace{

    template<typename OutgoingHandler>
    data_ptr incoming_holder_perform_once(
      data_ptr d, io_id_t io_id, OutgoingHandler outgoing_handler, 
      std::function<void(incoming_holder, io_id_t, OutgoingHandler)> incoming_handler 
    )
    {
      incoming_holder holder(std::move(d));
      try
      {
        d = holder.parse();
      }
      catch(const json::json_error& er)
      {
        JSONRPC_LOG_WARNING( "jsonrpc::incoming_holder: parse error: " << holder.error_message(er) )
        send_error( std::move(holder), std::make_unique<parse_error>(), std::move(outgoing_handler));
        return nullptr;
      }

      if ( holder.is_valid() )
      {
        incoming_handler( std::move(holder), io_id, std::move(outgoing_handler));
      }
      else
      {
        JSONRPC_LOG_WARNING( "jsonrpc error: " << holder.str() );
        send_error( std::move(holder), std::make_unique<invalid_request>(), std::move(outgoing_handler));
      }
      return std::move(d);
    }
  
    template<typename OutgoingHandler>
    void perform_impl_t(
      data_ptr d, io_id_t io_id, OutgoingHandler outgoing_handler, 
      std::function<void(incoming_holder, io_id_t, OutgoingHandler)> incoming_handler )
    {
      try
      {
        while (d != nullptr)
        {
          d = incoming_holder_perform_once(std::move(d), io_id, outgoing_handler, incoming_handler);
        }
      }
      catch(const std::exception& ex)
      {
        JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << ex.what() )
        send_error( std::move(incoming_holder(nullptr)),  std::make_unique<server_error>(), std::move(outgoing_handler));
      }
      catch(...)
      {
        JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << "unhandler exception" )
        send_error( std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(outgoing_handler));
      }
    }
  }


void send_error( incoming_holder holder, std::unique_ptr<error> err, outgoing_handler_t outgoing_handler)
{
  typedef outgoing_error_json< error_json > message_json;
  outgoing_error<error> error_message;
    
  error_message.error = std::move(err);
  auto id_range = holder.raw_id();
  if ( id_range.first != id_range.second )
  {
    error_message.id = std::make_unique<data_type>( id_range.first, id_range.second );
  }

  auto d = holder.detach();
  if ( d == nullptr )
    d = std::make_unique<data_type>();
  d->clear();
  d->reserve(80);
  typename message_json::serializer()(error_message, std::inserter( *d, d->end() ));
  
  outgoing_holder out(std::move(d));
  outgoing_handler( std::move(out) );
}

void send_error( incoming_holder holder, std::unique_ptr<error> err, iow::io::outgoing_handler_t outgoing_handler)
{
  typedef outgoing_error_json< error_json > message_json;
  outgoing_error<error> error_message;
    
  error_message.error = std::move(err);
  auto id_range = holder.raw_id();
  if ( id_range.first != id_range.second )
  {
    error_message.id = std::make_unique<data_type>( id_range.first, id_range.second );
  }

  auto d = holder.detach();
  if ( d == nullptr )
    d = std::make_unique<data_type>();
  d->clear();
  d->reserve(80);
  typename message_json::serializer()(error_message, std::inserter( *d, d->end() ));

  outgoing_handler( std::move(d) );
}



void perform(
    data_ptr d, io_id_t io_id, outgoing_handler_t outgoing_handler, 
    std::function<void(incoming_holder, io_id_t, outgoing_handler_t)> incoming_handler )
{
  perform_impl_t( std::move(d), io_id, std::move(outgoing_handler), std::move(incoming_handler));
  /*
  try
  {
    while (d != nullptr)
    {
      d = incoming_holder_perform_once(std::move(d), io_id, outgoing_handler, incoming_handler);
    }
  }
  catch(const std::exception& ex)
  {
    JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << ex.what() )
    send_error( std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(outgoing_handler));
  }
  catch(...)
  {
    JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << "unhandler exception" )
    send_error( std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(outgoing_handler));
  }
  */
}

void perform(
  data_ptr d, io_id_t io_id, ::iow::io::outgoing_handler_t outgoing_handler, 
  std::function<void(incoming_holder, io_id_t, ::iow::io::outgoing_handler_t)> incoming_handler
)
{
  perform_impl_t( std::move(d), io_id, std::move(outgoing_handler), std::move(incoming_handler));
  
  //perform_impl_t< ::iow::io::outgoing_handler_t>( std::move(d), io_id, std::move(outgoing_handler), std::move(incoming_handler));
  /*
  try
  {
    while (d != nullptr)
    {
      d = incoming_holder_perform_once(std::move(d), io_id, outgoing_handler, incoming_handler);
    }
  }
  catch(const std::exception& ex)
  {
    JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << ex.what() )
    send_error( std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(outgoing_handler));
  }
  catch(...)
  {
    JSONRPC_LOG_ERROR( "jsonrpc::engine: server error: " << "unhandler exception" )
    send_error( std::move(incoming_holder(nullptr)), std::make_unique<server_error>(), std::move(outgoing_handler));
  }
  */
}

}}}
