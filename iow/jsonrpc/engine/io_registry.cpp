
#include <wfc/jsonrpc/service/io_registry.hpp>
#include <wfc/logger.hpp>

namespace wfc{ namespace jsonrpc{

  
io_registry::io_registry()
  : _tmp_time(0)
  , _call_id_counter(0)
{
  
}
  
void io_registry::set_io(io_id_t io_id, std::shared_ptr<handler_interface> jsonrpc_handler, outgoing_handler_t outgoing_handler)
{
  lock_guard lk(_mutex);
  auto result = _io_map.insert( std::make_pair( io_id, io_info(jsonrpc_handler, outgoing_handler) ) );
  if ( !result.second )
  {
    DAEMON_LOG_FATAL("jsonrpc::io_registry::set_io io_id=" << io_id << " exists");
    abort();
  }
}

bool io_registry::has_io( io_id_t io_id ) const
{
  read_lock lk(_mutex);
  return _io_map.find(io_id)!=_io_map.end();
}
  
auto io_registry::erase_io( io_id_t io_id ) 
-> std::shared_ptr<handler_interface>
{
  lock_guard lk(_mutex);
  
  std::shared_ptr<handler_interface> result;
  
  
  auto itr = _io_map.find(io_id);
  if ( itr != _io_map.end() )
  {
    result = itr->second.jsonrpc_handler;
    for ( auto& r : itr->second.result_map )
    {
      this->_call_io_map.erase(r.first);
    }
    this->_io_map.erase(itr);
  }
  return result;
}
  

auto io_registry::add_result_handler(io_id_t io_id, result_handler_t result_handler)
  ->std::pair< call_id_t, outgoing_handler_t >
{
  // this->check();

  std::pair<call_id_t, outgoing_handler_t> result(-1, nullptr);
  
  lock_guard lk(_mutex);
  
  
  auto itr = this->_io_map.find(io_id);
  if (itr!=this->_io_map.end())
  {
    result.first = ++this->_call_id_counter;
    result.second = itr->second.outgoing_handler;
    itr->second.result_map.insert( std::make_pair(result.first, result_handler));
    this->_call_io_map[result.first] = io_id;
  }
  return result;
}

auto io_registry::detach_result_handler(call_id_t call_id)
 -> io_registry::result_handler_t
{
  lock_guard lk(_mutex);
  
  auto itr = _call_io_map.find(call_id);
  if ( itr != _call_io_map.end() )
  {
    auto itr2 = _io_map.find(itr->second);
    if ( itr2 != _io_map.end() )
    {
      auto itr3 = itr2->second.result_map.find(call_id);
      if ( itr3 != itr2->second.result_map.end() )
      {
        auto result = itr3->second;
        itr2->second.result_map.erase(itr3);
        _call_io_map.erase(itr);
        return result;
      }
      else
      {
        _call_io_map.erase(itr);
        DAEMON_LOG_ERROR("jsonrpc::service: jsonrpc id=" << call_id << " not found in response list");
      }
    }
    else
    {
      _call_io_map.erase(itr);
      COMMON_LOG_WARNING("jsonrpc::service: io id=" << itr->second << " not found");
    }
  }
  else
  {
    COMMON_LOG_WARNING("jsonrpc::service: jsonrpc call_id=" << call_id << " not found");
  }
  return nullptr;
}
  

auto io_registry::get_jsonrpc_handler(io_id_t io_id) const
-> std::shared_ptr<handler_interface> 
{
  read_lock lk(_mutex);
  
  auto itr = _io_map.find(io_id);
  
  return itr != _io_map.end() 
    ? itr->second.jsonrpc_handler
    : nullptr;
}
  
auto io_registry::get_outgoing_handler(io_id_t io_id) const
 -> outgoing_handler_t
{
  read_lock lk(_mutex);
  
  auto itr = _io_map.find(io_id);
  
  return itr != _io_map.end() 
    ? itr->second.outgoing_handler
    : nullptr;
}

void io_registry::clear()
{
  lock_guard lk(_mutex);
  
  _io_map.clear();
  _call_io_map.clear();
}

void io_registry::check() const
{
  time_t now = time(0);
  if ( _tmp_time  < now )
  {
    // Здесь был trace _io_map и _call_io_map
    _tmp_time = now;
  }
}

}} // wfc


