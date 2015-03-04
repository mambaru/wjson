#pragma once

#include <thread>
#include <memory>

  
namespace iow{ 

template<typename Options>  
struct io_thread_options
  : Options
{
  
};
  
template<typename Subject >
class io_thread
{
public:
  typedef io_thread self;
  typedef Subject subject_type;
  typedef std::shared_ptr<subject_type>  subject_ptr;
  typedef typename subject_type::io_service_type io_service_type;
  typedef typename subject_type::options_type options_type;
  
  ~io_thread()
  {
    if ( _thread.joinable() )
      this->stop();
  }
  
  io_thread(io_service_type& io_service, const options_type& opt )
    : _io_service_ptr(nullptr)
  {
    _subject = std::make_shared<subject_type>(io_service, opt);
  }

  // Запуск в отдельном потоке 
  io_thread(const options_type& opt )
    : _io_service_ptr( std::make_shared<io_service_type>() )
  {
    _subject = std::make_shared<subject_type>(*_io_service_ptr, opt);
  }
  
  subject_ptr get() const 
  {
    return _subject;
  }
  
  void start()
  {
    if ( _io_service_ptr!=nullptr )
    {
      _thread = std::thread([this]()
      {
        typename io_service_type::work wrk( *_io_service_ptr );
        this->_subject->start();
        this->_io_service_ptr->run();
      });
    }
    else
    {
      this->_subject->start();
    }
  }
  
  void stop()
  {
    this->_subject->stop();
    if ( _thread.joinable() )
    {
      _io_service_ptr->stop();
      _thread.join();
    }
  }

private:
  
  std::thread _thread;
  io_service_ptr _io_service_ptr;
  subject_ptr _subject;
  
};

}
