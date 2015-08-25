#pragma once

#include <iow/ip/tcp/acceptor/acceptor.hpp>
#include <iow/ip/tcp/server/options.hpp>
#include <iow/io/basic/tags.hpp>
#include <iow/asio.hpp>
#include <fas/aop.hpp>

namespace iow{ namespace io{ namespace mtio{

struct _context_;

/**
 
 descriptor   service  
    dub       service
    dub       service
    dub       service
    dub       service
 
 
 
 */

template<typename HolderType>
struct context
{
  typedef HolderType holder_type;
  typedef std::shared_ptr<holder_type> holder_ptr;
  typedef ::iow::asio::io_service io_service_type;
  typedef std::shared_ptr<io_service_type> io_service_ptr;
  typedef std::list< std::thread > thread_list;
  typedef std::list< io_service_ptr > io_service_list;
  typedef std::list< holder_ptr> holder_list;
  
  // Потоков на сервис
  int threads_per_service;
  // Дескрипторов на сервис
  int descriptor_per_service;
  // Количество сервисов
  int service_count;
  
  thread_list     threads;
  io_service_list services;
  holder_list     holders;
  
};

struct _server_start_;
struct ad_server_start
{
  template<typename T>
  void operator()( T& )
  {
    typedef typename T::aspect::template advice_cast<_context_>::type context_type;
    typedef typename context_type::io_service_type io_service_type;
    typedef typename context_type::holder_type holder_type;
    typedef typename holder_type::descriptor_type descriptor_type;
    
    auto& cntx = t.get_aspect().template get<_context_>();
    for ( int s = 0; s  < cntx.service_count; ++s)
    {
      auto srv = std::make_shared<io_service_type>();
      cntx.services.push_back(srv);
      auto holder = std::make_shared<holder_type>( std::move( descriptor_type(*srv)));
      cntx.holders.push_back(holder);
      holder->start();
      for ( int h = 1; h < cntx.descriptor_per_service; ++h)
      {
        auto holder2 = std::make_shared<holder_type>( std::move( holder->dup() ));
        holder2->start();
        cntx.holders.push_back( holder2 );
      }
      
      for ( int t = 0; t < cntx.threads_per_service; ++t)
      {
        cntx.threads.push_back([srv](){
          srv->run();
        });
      }
    }
  }
};

struct ad_initialize
{
  template<typename T, typename O>
  void operator()( T& t, O&& opt )
  {
    auto& cntx = t.get_aspect().template get<_context_>();
    cntx.thread_count = opt.threads;
  }
};


typedef ::iow::ip::tcp::acceptor::acceptor<> acceptor;

template<typename AcceptorType = acceptor>
struct aspect1: fas::aspect<
  fas::type< _io_service_type_, ::iow::asio::io_service>,
  fas::type< ::iow::io::_options_type_, options>,
  fas::value< _context_, context<AcceptorType> >,
  fas::advice< ::iow::io::_initialize_, ad_initialize>,
  fas::advice< _server_start_, ad_server_start>,
  fas::group< ::iow::io::_after_start_, _server_start_ >
>{};
  
}}}
