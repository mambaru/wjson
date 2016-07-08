//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/serializer/quoted.hpp>
#include <string>

namespace wjson{

template<typename J, bool SerQ, bool ReqQ>
struct raw_quoted 
{
  typedef typename J::target target;
  typedef serializerRQ< J, SerQ, ReqQ > serializer;
};

template<typename J, bool SerQ , bool ReqQ , int R >
struct quoted
{
  typedef typename J::target target;
  typedef serializerQ< J, value<std::string>, SerQ, ReqQ, R> serializer;
};

}

