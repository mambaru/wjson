#include <iostream>
#include <iow/io/io_base.hpp>
#include <iow/io/basic/aspect.hpp>
#include <iow/io/flow/aspect.hpp>
#include <iow/memory.hpp>

#include <fas/testing.hpp>

#include <vector>
#include <memory>
#include <list>
typedef std::vector<char> data_type;
typedef std::unique_ptr<data_type> data_ptr;
struct _handler_;
struct ad_handler
{
  template<typename T>
  void operator()(T& t, typename T::input_t d)
  {
    t.result += std::string(d->begin(), d->end());
  }
};

struct ad_some
{
  template<typename T>
  void operator()(T& t, typename T::input_t d)
  {
    auto tmp = std::move(t.input.front());
    t.input.pop_front();
    std::copy(tmp->begin(), tmp->end(), d->begin());
    d->resize(tmp->size());
    t.get_aspect().template get< ::iow::io::flow::_complete_>()(t, std::move(d));
  }
};



struct ad_factory
{
  template<typename T>
  typename T::input_t operator()(T& t)
  {
    
    if ( t.input.empty() )
      return nullptr;
    return std::make_unique<data_type>(100);
    /*
    auto d = std::move(t.input.front());
    t.input.pop_front();
    return d;*/
    //return std::make_unique<data_type>(100);
  }
};


class flow
  : public ::iow::io::io_base< fas::aspect< 
      ::iow::io::basic::aspect<>::advice_list,
      ::iow::io::flow::aspect<_handler_,data_ptr>::advice_list,
      fas::advice<_handler_, ad_handler>,
      fas::advice<::iow::io::flow::_factory_, ad_factory>,
      fas::advice<::iow::io::flow::_some_, ad_some>
    > >
{
public:
  typedef data_ptr input_t;
  void start() { this->start_(*this, fas::empty_type() ); }
  void add(std::string val)
  {
    input.push_back( std::make_unique<data_type>(val.begin(), val.end()) );
  }
  std::list<data_ptr> input;
  std::string result;
};

UNIT(flow, "")
{
  using namespace fas::testing;
  flow f;
  f.start();
  std::cout << f.result << std::endl;
  t<<nothing;
}

BEGIN_SUITE(flow,"")
  ADD_UNIT(flow)
END_SUITE(flow)

