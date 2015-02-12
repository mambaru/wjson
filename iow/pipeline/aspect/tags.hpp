#pragma once

/*
Базовый конвейер

  _after_start_ -> _run_line_ 
                       |
               |---->_read_more_  -> _make_buffer_ 
               |        |
               |    _read_some_                                       _buffer_pool_
               |        : асинхронно всегда                                  create()
               |    _read_ready_  -> _on_read_(указатель на буффер)          free()
               |        |
               ---- _read_handler_ -> _incoming_ 
                                            : асинхронно или синхронно
                      _outgoing_      <---------                  
                          |
                      _write_more   -> _prepare_buffer_
                          | если есть что                                    ^
                      _write_some_                                           |
                          |                                                  |
                      _write_ready_ -> _on_write_                            |
                              |                                              |
                       _free_buffer_ -----------------------------------------
                         
*/

namespace iow{
  
struct _run_line_;
struct _read_more_;
struct _make_buffer_; 
struct _read_some_;
struct _buffer_pool_;
struct _read_ready_;
struct _on_read_;
struct _read_handler_;
struct _incoming_;
struct _outgoing_;  
struct _write_more_;
struct _prepare_buffer_;
struct _write_some_;
struct _write_ready_;
struct _on_write_;
struct _free_buffer_;

}
