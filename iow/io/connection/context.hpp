#pragma once

#include <iow/io/aux/data_pool.hpp>
#include <iow/io/descriptor/context.hpp>

#include <vector>
#include <memory>

namespace iow{ namespace io{ namespace connection{

namespace base{
  using context = ::iow::io::descriptor::context<
    size_t, 
    std::vector<char>, 
    std::unique_ptr<std::vector<char> >
  >;
}

struct context: base::context
{
  typedef base::context::data_type data_type;
  typedef ::iow::io::data_pool< data_type > pool_type;
  typedef std::shared_ptr< pool_type> pool_ptr;
  pool_ptr pool;
};

}}}
