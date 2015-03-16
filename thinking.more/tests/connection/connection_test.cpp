#include <iostream>
#include <boost/concept_check.hpp>
#include <iow/connection/connection.hpp>
#include <iow/types.hpp>
#include <fas/testing.hpp>


typedef ::iow::connection< 
  ::iow::domain::local,
  ::iow::trasport::local,
  ::iow::type::stream,
  ::iow::layer::echo
> echo_conn_t;

class echo_conn
  : public echo_conn_t
{
public:
  IOW_INHERITING_CONSTRUCTORS(echo_conn, echo_conn_t)
};


UNIT(stream_echo, "")
{
  using namespace fas::testing;
  typedef echo_conn conn_type;
  typedef std::shared_ptr<echo_conn> conn_ptr;
  typedef conn_type::io_service_type io_service_type;
  typedef conn_type::options_type options_type;
  io_service_type io;
  options_type opt;
  auto conn=std::make_shared<conn_type>(io,opt);
  conn->descriptor().assign( ::dup(STDIN_FILENO) );
  conn->context_().status = true;
  conn->start();
  io_service_type::work wrk(io);
  io.run();
  //posix::stream_descriptor in(my_io_service, ::dup(STDIN_FILENO));

  t << nothing;
}

BEGIN_SUITE(connection,"connection")
  ADD_UNIT(stream_echo)
END_SUITE(connection)

BEGIN_TEST
  RUN_SUITE(connection)
END_TEST
