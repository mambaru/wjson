#pragma once

#include <iow/io/strand/options.hpp>
#include <iow/workflow/delayed_queue.hpp>
#include <iow/asio.hpp>

#include <memory>
#include <string>
#include <atomic>

namespace iow{ namespace io{ namespace strand{

struct context: options
{
  //typedef ::iow::asio::io_service io_service_type;
  //typedef io_service_type::strand strand_type;
  typedef ::iow::delayed_queue strand_type;
  typedef std::shared_ptr<strand_type> strand_ptr;
  typedef std::atomic<size_t> counter_type;
  typedef std::shared_ptr<counter_type> counter_ptr;

  size_t maxsize = 0;
  size_t wrnsize = 0;
  time_t wrntime = 0;
  counter_ptr counter;
  strand_ptr strand;
};

}}} // iow::io::strand
