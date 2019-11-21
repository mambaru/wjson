//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2019
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#if __cplusplus >= 201103L

namespace wjson{

  template<typename Itr>
  inline int multi_deserializer(Itr beg, Itr end, json_error* e)
  {
    parser::parse_value(beg, end, e);
    return 0;
  }

  template<typename Arg, typename ...Args, typename Itr>
  inline int multi_deserializer(Itr beg, Itr end, typename Arg::target& arg, typename Args::target& ... args, json_error* e)
  {
    json_error er;
    typename Arg::serializer()(arg, beg, end, &er);
    if ( !er ) return 0;
    return 1 + multi_deserializer<Args...>(beg, end, args..., e);
  }
}

#endif
