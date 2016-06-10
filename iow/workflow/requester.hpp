#pragma once

#include <memory>
#include <functional>

namespace iow{

class requester
{
public:
  typedef std::function<void(bool)> handler_callback;

  template< typename Req, typename Res, typename Q, typename I, typename MemFun, typename Handler >
  static auto make(std::shared_ptr<Q> q, std::shared_ptr<I> i, MemFun mem_fun, Handler result_handler ) 
    -> std::function<void( handler_callback)>
  {
    std::weak_ptr<Q> wq = q;
    std::weak_ptr<I> wi = i;
    return requester::make_requester_<Req, Res>(wq, wi, std::move(mem_fun), std::move(result_handler) );
  }

private:

  template< typename Req, typename Res, typename Q, typename I, typename MemFun, typename Handler >
  static auto make_requester_(std::weak_ptr<Q> wq, std::weak_ptr<I> wi, MemFun mem_fun, Handler result_handler ) 
    -> std::function<void( handler_callback)>
  {
    return [wq, wi, mem_fun, result_handler](handler_callback timer_handler)
    {
      auto req = result_handler( nullptr );
      requester::send_request_<Res>( wq, std::move(req), wi, mem_fun, std::move(result_handler), std::move(timer_handler) );
    };
  }

  template< typename Res, typename Q, typename ReqPtr, typename I, typename MemFun, typename ResultHandler >
  static void send_request_(std::weak_ptr<Q> wq, ReqPtr req, std::weak_ptr<I> wi, MemFun mem_ptr, ResultHandler result_handler, handler_callback timer_handler)
  {
    auto i = wi.lock();
    if ( i == nullptr )
    {
      timer_handler(false);
      return;
    }

    auto callback = [ wq, wi, mem_ptr, result_handler, timer_handler]( std::unique_ptr<Res> res)
    {

      if ( auto pq = wq.lock() )
      {
        if ( res == nullptr )
        {
          // is service unavailable
          timer_handler(true);
          return;
        }
        
        auto pres = std::make_shared< std::unique_ptr<Res> >( std::move(res) );
        pq->post([pres, wq, wi, mem_ptr, result_handler, timer_handler]()
        {
          if ( auto pq = wq.lock() )
          {
            if ( auto req = result_handler( std::move(*pres) ) )
            {
              send_request_<Res>( wq, std::move(req), wi, mem_ptr, std::move(result_handler), std::move(timer_handler) );
            }
            else
            {
              timer_handler(true);
            }
          }
        });
      }
    };
    (i.get()->*mem_ptr)( std::move(req), callback );
  }
};


}
