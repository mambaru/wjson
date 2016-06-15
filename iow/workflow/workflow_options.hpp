#pragma once
#include <memory>

namespace iow{

class workflow;

struct workflow_options
{
  std::string id;
  bool use_io_service = false;
  int  threads = 0;

  size_t maxsize = 1024*1024*128;
  size_t wrnsize = 1024*1024;
  time_t show_wrn_ms = 1000;

  time_t post_delay_ms = 0; // задержка в миллисекундах 
  size_t rate_limit = 0;    // ограничение скорости (в сообщениях в секунду)

  // для таймера проверки wrnsize
  std::shared_ptr< ::iow::workflow> workflow_ptr;
  std::function<bool()> handler = nullptr;
};

}
