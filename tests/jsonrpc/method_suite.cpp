#include <iostream>
#include <iow/jsonrpc/method.hpp>
#include <iow/jsonrpc/noparams_json.hpp>

#include <fas/testing.hpp>

#include "req.hpp"


UNIT(outgoing_unit, "")
{
  using namespace fas::testing;
  using namespace ::iow::jsonrpc;
  
  // int a[6] = { 0, 0, 15, 0, 29, 0 };

  // int a[6] = { [4] = 29, [2] = 15 };
  //t << message("bad parse: ") << a[0];
  t << nothing;
}


UNIT(incoming_unit, "")
{
  using namespace fas::testing;
  using namespace ::iow::jsonrpc;
  std::string data = "{\"method\":\"test\"}";
  
  for (auto r : good_parse )
  {
    incoming_holder h( std::make_unique<data_type>(r.begin(), r.end()) );
    t << message("good parse: ") <<  r;
    t << flush;
    h.parse(nullptr);
  }
  
  for (auto r : bad_parse )
  {
    incoming_holder h( std::make_unique<data_type>(r.begin(), r.end()) );
    try
    {
      t << message("bad parse: ") <<  r;
      h.parse(nullptr);
      t << fatal("GOOD PARSE");
    }
    catch(const iow::json::json_error& e)
    {
      t << message("parse error: ") <<  h.error_message(e);
    }
  }

  /*
  t << message("-------------------------");
  for (auto r : bad_request )
  {
    incoming_holder h( std::make_unique<data_type>(r[0].begin(), r[0].end()) );
    t << message("bad request: ") <<  r[0];
    h.parse();
    t << is_true<assert>( h.has_params() ) << FAS_TESTING_FILE_LINE;
    try
    {
      h.get_params<noparams_json>();
      t << fatal("GOOD REQUEST");
    }
    catch(const iow::json::json_error& e)
    {
      t << message("parse error: ") <<  h.error_message(e);
    }
  }
  */

  t << nothing;
}

JSONRPC_TAG(method1)

#include <iow/jsonrpc/method/aspect/name.hpp>

UNIT(method_unit, "")
{
  using namespace fas::testing;
  using namespace ::iow::jsonrpc;

  //basic_method< name<_method1_> > m;
  incoming_holder ih(nullptr, false);
  // m(m, std::move(ih), nullptr);
  //method< name<_method1_> > m;
  //m(std::move(ih));
  t << nothing;
}

BEGIN_SUITE(method_suite, "")
  ADD_UNIT(outgoing_unit)
  ADD_UNIT(incoming_unit)
  ADD_UNIT(method_unit)
END_SUITE(method_suite)