#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <iow/memory.hpp>



namespace iow{ namespace io{

typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;

typedef size_t io_id_t;

typedef std::function< void(data_ptr) > outgoing_handler_t;
typedef std::function< void(data_ptr, io_id_t, outgoing_handler_t )> incoming_handler_t;
typedef std::function< void(io_id_t, outgoing_handler_t) > startup_handler_t;
typedef std::function< void(io_id_t) > shutdown_handler_t;

inline data_ptr make(const std::string& str )
{
  return std::make_unique<data_type>(str.begin(), str.end() );
}

}}

#include <ostream>
#include <string>

namespace std{
  inline std::ostream& operator << (std::ostream& os, const ::iow::io::data_ptr& d)
  {
    if ( d!=nullptr )
    {
      os << std::string( d->begin(), d->end() );
    }
    else
    {
      os << "nullptr";
    }
    return os;
  }
}
