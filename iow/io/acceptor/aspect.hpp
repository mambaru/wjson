#pragma once

#include <iow/io/acceptor/tags.hpp>
#include <iow/io/acceptor/context.hpp>
#include <iow/io/acceptor/options.hpp>
#include <iow/io/acceptor/ad_confirm.hpp>
#include <iow/io/acceptor/ad_accept_handler.hpp>
#include <iow/io/acceptor/ad_make_handler.hpp>
#include <iow/io/acceptor/ad_async_accept.hpp>
#include <iow/io/acceptor/ad_next.hpp>
#include <iow/io/acceptor/ad_initialize.hpp>
#include <iow/io/acceptor/ad_resolve_and_start.hpp>
#include <iow/io/acceptor/ad_listen.hpp>

#include <iow/io/basic/aspect.hpp>
#include <iow/io/reader/aspect.hpp>
#include <iow/io/reader/asio/aspect.hpp>
#include <iow/io/writer/asio/aspect.hpp>
#include <iow/io/stream/aspect.hpp>
#include <iow/io/descriptor/manager.hpp>
#include <iow/io/descriptor/ad_initialize.hpp>
#include <iow/asio.hpp>
#include <iow/system.hpp>
#include <fas/aop.hpp>
#include <boost/concept_check.hpp>
#include <list>

namespace iow{ namespace io{ namespace acceptor{

// в tcp
struct aspect_base: fas::aspect<
  ::iow::io::basic::aspect<std::recursive_mutex>::advice_list,
  ::iow::io::reader::aspect,
  fas::advice< ::iow::io::reader::_next_, ad_next>,
  fas::advice< ::iow::io::_initialize_, ad_initialize>,
  fas::advice< ::iow::io::reader::_make_handler_, ad_make_handler>,
  fas::advice< ::iow::io::reader::_read_some_, ad_async_accept>,
  fas::advice< ::iow::io::reader::_confirm_,  ad_confirm>,
  fas::stub<   ::iow::io::reader::_handler_>,
  fas::advice< _accept_handler_, ad_accept_handler>,
//  fas::group<  ::iow::io::_after_start_, _resolve_and_start_>,
//  fas::advice< _resolve_and_start_, ad_resolve_and_start>,
//  fas::advice< _acceptor_start_, ad_acceptor_start>
  fas::advice< _listen_, ad_listen>
>{};

template<typename ConnectionType>
struct aspect: fas::aspect<
  aspect_base::advice_list,
  fas::value< _context_, context<ConnectionType> >
>{};

}}}
