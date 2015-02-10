#pragma once

namespace wfc{ namespace io{

struct _descriptor_type_;
struct _io_service_type_;
//struct _error_code_;

struct _context_;
struct _data_type_;
struct _strand_;
struct _strand_type_;

struct _mutex_type_;
struct _lock_guard_;
struct _read_lock_;

struct _owner_;
struct _owner_type_;


struct _post_;
struct _wrap_;
struct _dispatch_;

struct _on_create_;
struct _pre_start_;
struct _on_start_;
struct _on_stop_;
/*
struct _stop_;
struct _after_stop_;
struct _before_stop_;
*/


// for user --> <_incoming_, (){ t->_outgoing_ }>
struct _incoming_;
struct _outgoing_;
struct _options_type_;
//struct _transfer_handler_;

// Descriptor holder
struct _status_;


}}
