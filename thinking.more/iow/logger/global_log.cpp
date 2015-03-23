//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include "global_log.hpp"
#include "logstream.hpp"

namespace iow{

static log_writer static_writer = nullptr;
  
void init_log(log_writer writer)
{
  static_writer = writer;
}
  
logstream global_log(const std::string& name, const std::string& type)
{
  return logstream( 
    name, 
    type, 
    static_writer
  );
}

}