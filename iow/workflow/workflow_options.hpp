#pragma once
#include <iow/workflow/queue_options.hpp>

namespace iow{

struct workflow_options: queue_options
{
  bool use_io_service = false;
  int  threads = 0;
  
  time_t post_delay_ms = 0; // задержка в миллисекундах 
  size_t rate_limit = 0;    // ограничение скорости (в сообщениях в секунду)
};

}
