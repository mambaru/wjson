#pragma once

#include <iow/pipeline/pipeline_context.hpp>
#include <map>
#include <vector>

namespace iow{

template<typename DescType >
struct idescriptor_manager
{
  typedef DescType descriptor_type;
  typedef std::shared_ptr<descriptor_type> descriptor_ptr;
  virtual descriptor_ptr create()  = 0;
};



template<typename DescType>
class descriptor_manager
  : public idescriptor_manager<DescType>
{
  typedef DescType descriptor_type;
  typedef typename descriptor_type::startup_handler_t  startup_handler_t;
  typedef typename descriptor_type::shutdown_handler_t shutdown_handler_t;
  typedef typename descriptor_type::incoming_handler_t incoming_handler_t;
  
  typedef std::shared_ptr<descriptor_type> descriptor_ptr;
  typedef std::map<io_id_t, descriptor_ptr> descriptor_map;
  typedef std::vector<descriptor_ptr> descriptor_pool;

  typedef std::recursive_mutex mutex_type;
public:
  
  virtual descriptor_ptr create()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _pool.empty()
    return nullptr;
  };

  void set_startup(startup_handler_t handler)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _pool.clear();
    
  }

  void set_shutdown(shutdown_handler_t handler)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _pool.clear();
  }

  void set_incoming(incoming_handler_t handler)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _pool.clear();
  }

private:
  descriptor_pool _pool;
  descriptor_map  _map;
  startup_handler_t  _startup_handler;
  shutdown_handler_t _shutdown_handler;
  incoming_handler_t _incoming_handler;
  mutex_type _mutex;
};

}
