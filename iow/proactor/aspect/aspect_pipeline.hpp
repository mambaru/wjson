#pragma once

#include <iow/proactor/aspect/tags.hpp>
#include <iow/proactor/aspect/ad_run_line.hpp>
#include <iow/proactor/aspect/ad_read_more.hpp>
//#include <iow/pipeline/aspect/ad_read_some.hpp>
#include <iow/proactor/aspect/ad_read_ready.hpp>
#include <iow/proactor/aspect/ad_read_handler.hpp>

#include <fas/aop.hpp>
/*
Базовый конвейер

  _after_start_ -> _run_line_ 
                       |
               |---->_read_more_  -> _make_data_ 
               |        |
               |    _read_some_ [здесь цепочка]                             _buffer_pool_
               |        : асинхронно всегда                                  create()
               |    _read_ready_  -> _on_read_(указатель на буффер)          free()
               |        |
               ---- _read_handler_ -> _incoming_                               ^
                                            : асинхронно или синхронно         |
                      _outgoing_      <---------                               |
                          |                                                    |
если все (nullptr) |->_write_more   -> _prepare_buffer_  -----------------------
                   |      | если есть что                                    
       если не все |->_write_some_   [здесь цепочка]                                           
                   |       |                                                  
                   ---_write_ready_ -> _on_write_                            
                          |                                                  
                      _free_buffer_ 
                         
*/

namespace iow{
  
struct aspect_pipeline: fas::aspect<
  fas::advice< _run_line_, ad_run_line >,
  fas::advice< _read_more_, ad_read_more >,
  fas::advice< _read_ready_, ad_read_ready >,
  fas::advice< _read_handler_, ad_read_handler >,
  fas::alias< _after_start_, _run_line_>
>{};

}
