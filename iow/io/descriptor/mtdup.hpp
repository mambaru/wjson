#pragma once
#include <memory>
#include <mutex>
#include <list>
#include <thread>

namespace iow{ namespace io{ namespace descriptor{

template<typename Holder>
class mtdup
{
public:
  typedef Holder holder_type;
  typedef typename holder_type::descriptor_type descriptor_type;
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef std::list<holder_ptr> holder_list;
  typedef std::list<std::thread> thread_list;
  typedef std::recursive_mutex mutex_type;

  mtdup(descriptor_type&& desc)
    : _origin( std::make_shared<holder_type>( std::forward<descriptor_type>(desc)))
  {}

  holder_ptr origin() const { return _origin;}

  template<typename Opt>
  void start(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);

  }

  template<typename Opt>
  void reconfigure(Opt&& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);

  }

  void stop()
  {
    std::lock_guard<mutex_type> lk(_mutex);

  }

private:
  mutex_type _mutex;
  holder_ptr _origin;
  holder_list _dup_list;
};

}}}