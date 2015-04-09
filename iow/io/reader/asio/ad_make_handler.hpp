struct ad_make_handler
{
  template<typename T, typename P>
  void operator()(T& t, const P& p)
  {
    std::weak_ptr<T> wthis = t.shared_from_this();
    return t.wrap([wthis, p]( ::iow::system::error_code ec , std::size_t bytes_transferred )
    {
      if ( auto pthis = wthis.lock() )
      {
        std::lock_guard<typename T::mutex_type> lk(pthis->mutex());
        pthis->get_aspect().template get<_handler_>()(*pthis, std::move(p), std::move(ec), bytes_transferred);
      }
    });
  }
};
