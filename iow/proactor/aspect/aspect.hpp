#pragma once

#include <iow/proactor/aspect/tags.hpp>
#include <iow/proactor/aspect/ad_run.hpp>
#include <iow/proactor/aspect/ad_more.hpp>
//#include <iow/pipeline/aspect/ad_read_some.hpp>
#include <iow/proactor/aspect/ad_ready.hpp>
#include <iow/proactor/aspect/ad_handler.hpp>

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

namespace iow{ namespace proactor{
  
typedef fas::type_list_n<
  fas::advice< _run_, ad_run >,
  fas::advice< _more_, ad_more >,
  fas::advice< _ready_, ad_ready >,
  fas::advice< _handler_, ad_handler >,
  fas::alias< _after_start_, _run_>
>::type advice_list;  

struct aspect: fas::aspect<
  advice_list
>{};

}}
