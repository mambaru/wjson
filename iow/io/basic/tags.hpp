#pragma once

namespace iow{ namespace io{

// io_base advice
struct _reset_;
struct _start_;
struct _stop_;
struct _shutdown_;
struct _wrap_;     

// io_base values
struct _io_id_;   // TODO: del (context)
struct _status_;  // TODO: del (context)
struct _owner_;   // TODO: del (context)

namespace basic{
struct _create_id_;
struct _context_;
}



// io_base types
struct _mutex_type_; // TODO: del (context)
struct _options_type_;

// io_base events
struct _after_reset_;
struct _before_start_;
struct _after_start_;
struct _initialize_;
struct _before_stop_;
struct _after_stop_;
struct _on_error_;

}}

