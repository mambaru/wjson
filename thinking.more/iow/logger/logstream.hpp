//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/logger/log_writer.hpp>
#include <sstream>
#include <string>
#include <memory>
#include <iostream>

namespace iow{

class logstream
{
public:
  ~logstream()
  {
    if ( writer_ )
    {
      writer_(_name, _type, _ss.str());
    }
    else
    {
#ifndef WFC_DISABLE_CLOG
      std::clog << _name << " " << _type << " " << _ss.str();
#endif
    }
  }
  
  logstream(const logstream& ll)
    : _name(std::move(ll._name))
    , _type(std::move(ll._type))
    , _ss(ll._ss.str())
    , writer_(std::move(ll.writer_))
  {
  }
  
  logstream(const std::string& name, const std::string& type, log_writer writer)
    : _name(name)
    , _type(type)
    , writer_(writer)
  {
  }

  logstream(std::string&& name, std::string&& type, log_writer writer)
    : _name( std::move(name) )
    , _type( std::move(type) )
    , writer_(writer)
  {
  }

  std::string str() const
  {
    return _ss.str();
  }

  template<typename V>
  std::stringstream& operator << ( V v)
  {
    _ss<<v;
    return _ss;
  }

private:
  std::string _name;
  std::string _type;
  std::stringstream _ss;
  log_writer writer_;
};

}
