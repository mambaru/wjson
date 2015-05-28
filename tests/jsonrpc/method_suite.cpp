#include <iow/jsonrpc/method.hpp>

#include <fas/testing.hpp>

UNIT(incoming_unit, "")
{
  using namespace fas::testing;
  using namespace ::iow::jsonrpc;
  std::string data = "{\"method\":\"test\"}";
  incoming_holder h( std::make_unique<data_type>(data.begin(), data.end()) );
  h.parse();
  t << is_true<assert>( h.is_notify() ) << FAS_TESTING_FILE_LINE;
  t << nothing;
}

JSONRPC_TAG(method1)

#include <iow/jsonrpc/method/aspect/name.hpp>

UNIT(method_unit, "")
{
  using namespace fas::testing;
  using namespace ::iow::jsonrpc;
  method< name<_method1_> > m;
  incoming_holder ih(nullptr, false);
  //m(std::move(ih));
}

BEGIN_SUITE(method_suite, "")
  ADD_UNIT(incoming_unit)
  ADD_UNIT(method_unit)
END_SUITE(method_suite)