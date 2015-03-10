#pragma once

#include<iow/pipeline/pipeline.hpp>
#include <iow/basic/io_context.hpp>
#include <list>

namespace iow{
  
struct acceptor_context: ::iow::io_context
{
  
};

struct ad_make_descriptor
{
  template<typename T>
  auto operator()(T& t)
    -> typename T::data_ptr
  {
    std::cout << "Make descriptor..." << std::endl;
    typedef typename T::data_type descriptor_type;
    typedef typename T::data_ptr  descriptor_ptr;
    descriptor_ptr d = std::make_shared<descriptor_type>( t.get_io_service(), t.options_().connection );
    return d;
    
    /*
      typedef typename T::descriptor_type descriptor_type;
      return std::move( descriptor_type(t.get_io_service(), t.options() ) );
    */
  }
};

struct ad_accept
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr conn)
  {
    std::cout << "Accept..." << std::endl;
    //auto conn = t.get_aspect().template get<_make_buffer_>()(t);
    std::weak_ptr<T> wthis = t.shared_from_this();
    auto callback = t.owner_wrap_([wthis, conn]( boost::system::error_code ec )
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk( pthis->mutex() );
        static std::list< typename T::data_ptr > tmp;
        tmp.push_back(conn);
        std::cout << "Accept: " << ec.message() << std::endl;
        pthis->get_aspect().template get<_read_ready_>()( *pthis, std::move(conn) );
      }
    });
    
    t.descriptor().async_accept
    (
      conn->descriptor(),
      callback
    );
    
    /*
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    auto pthis=t.shared_from_this();
    auto callback =         [pthis, dd]( boost::system::error_code ec )
        { 
          typename T::lock_guard lk( pthis->mutex() );
          
          if ( !pthis->status() )
            return;

          if ( !pthis->descriptor().is_open() )
            return;
          
          pthis->get_aspect().template get< _outgoing_>()(*pthis, std::move(*dd), ec);
        };
    */
    //auto wcallback = t.owner().wrap(callback);

    
    /*
    t.mutex().unlock();
    t.descriptor().async_accept
    (
      **dd,
      callback
    );
    t.mutex().lock();
    */
  }
};

struct aspect_acceptor_base: fas::aspect< 
  fas::type< _context_type_, acceptor_context>
  ,fas::advice<_make_buffer_, ad_make_descriptor>
  , fas::advice<_read_some_, ad_accept>
> {};

template<typename A = fas::aspect<> >
class acceptor_base
  : public pipeline< typename fas::merge_aspect<A, aspect_acceptor_base>::type  >
{
public:
  typedef acceptor_base<A> self;
  typedef pipeline< typename fas::merge_aspect<A, aspect_acceptor_base>::type  > super;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::options_type options_type;
  
  typedef typename super::aspect::template advice_cast<_data_type_>::type data_type;
  typedef typename super::aspect::template advice_cast<_data_ptr_>::type data_ptr;

  
  acceptor_base(io_service_type& io, const options_type& opt)
    : super(io,  opt)
  {}
  // IOW_INHERITING_CONSTRUCTORS_T(acceptor_base, super)
};

namespace ip{ namespace tcp{

struct connection_options{};
  
struct acceptor_options
{
  size_t backlog = 1024;
  
  std::string host = "0.0.0.0";
  std::string port = "12345";
  
  connection_options connection;
};


struct _listen_;
struct ad_listen
{
  template<typename T>
  void operator()(T& t)
  {
    iow::asio::ip::tcp::resolver resolver( t.get_io_service() );
    iow::asio::ip::tcp::endpoint endpoint = *resolver.resolve(
    {
      t.options().host, 
      t.options().port
    });

    t.descriptor().open(endpoint.protocol());
    t.descriptor().set_option( iow::asio::ip::tcp::acceptor::reuse_address(true) );
    t.descriptor().bind(endpoint);
    t.descriptor().listen( t.options().backlog );
  }
};

  ///, fas::type< _data_type_, descriptor_holder<> >
   // TODO: временно
  //fas::type< _data_ptr_, std::shared_ptr< descriptor_holder<> > > // TODO: временно


struct connection_context{};

struct aspect_connection: fas::aspect<
  fas::type<_options_type_, connection_options>,
  fas::type<_context_type_, connection_context>,
  fas::type<_descriptor_type_, iow::asio::ip::tcp::socket>,
  fas::type< ::iow::_mutex_type_, std::mutex>
>{};

template<typename A = fas::aspect<> >
class connection
  : public ::iow::pipeline< typename fas::merge_aspect<A, aspect_connection>::type >
{
  typedef ::iow::pipeline< typename fas::merge_aspect<A, aspect_connection>::type > super;
  typedef connection<A> self;
public:
  connection(typename super::io_service_type& io, const typename super::options_type& opt)
    : super(io, opt)
  {
    
  }
};


struct aspect_acceptor
  : fas::aspect< 
      fas::type<_descriptor_type_, ::iow::asio::ip::tcp::acceptor >,
      fas::type<_options_type_, acceptor_options>,
      fas::type<_data_type_, connection<> >,
      fas::type<_data_ptr_, std::shared_ptr<connection<> > >,
      fas::type< ::iow::_mutex_type_, std::mutex>,
      fas::advice<_listen_, ad_listen>
    > 
{};

template<typename A = fas::aspect<> >
class acceptor final:
  public ::iow::acceptor_base< typename fas::merge_aspect< A, aspect_acceptor >::type >,
  public std::enable_shared_from_this< acceptor<A> >
{
public:
  typedef acceptor<A> self;
  typedef ::iow::acceptor_base< typename fas::merge_aspect< A, aspect_acceptor >::type > super;
  typedef typename super::io_service_type io_service_type;
  typedef typename super::options_type    options_type;
  typedef typename super::descriptor_type descriptor_type;
  
  IOW_INHERITING_CONSTRUCTORS_T(acceptor, super)
  IOW_IO_BASE_IMPL_T(super)

  void listen()
  {
    super::get_aspect().template get<_listen_>()(*this);
    
    /*
    boost::asio::ip::tcp::resolver resolver( super::get_io_service() );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(
    {
      super::options().host, 
      super::options().port
    });

    super::descriptor().open(endpoint.protocol());
    super::descriptor().set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
    super::descriptor().bind(endpoint);
    super::descriptor().listen( super::options().backlog );
    */
  }
};

}}
  
}