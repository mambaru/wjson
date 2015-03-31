
#include <iow/io/aux/read_buffer.hpp>
#include <fas/testing.hpp>
#include <cstring>

typedef std::vector<char> data_type;
typedef std::string sep_type;
typedef ::iow::io::read_buffer<data_type, sep_type> read_buffer;

UNIT(basic_test, "")
{
  using namespace fas::testing;
  read_buffer buf;
  auto tmp = std::make_unique<data_type>(3);
  auto att = buf.attach(std::move(tmp));
  
  t << is_true<assert>( att == nullptr) << FAS_TESTING_FILE_LINE;
  
  // Еще один attach() без next() запрещен
  tmp = std::make_unique<data_type>(3);
  att = buf.attach(std::move(tmp));
  t << is_true<assert>( att != nullptr) << FAS_TESTING_FILE_LINE;
  
  auto nxt1 = buf.next();
  t << is_true<assert>( nxt1.first != nullptr) << FAS_TESTING_FILE_LINE;
  t << is_true<assert>( nxt1.second == 3) << FAS_TESTING_FILE_LINE;
  
  // Еще один next() без confirm() запрещен
  auto nxt2 = buf.next();
  t << is_true<assert>( nxt2.first == nullptr) << FAS_TESTING_FILE_LINE;
  t << is_true<assert>( nxt2.second == 0) << FAS_TESTING_FILE_LINE;
  
  // После next() разрешен attach()
  tmp = std::make_unique<data_type>(3);
  att = buf.attach(std::move(tmp));
  t << is_true<assert>( att == nullptr) << FAS_TESTING_FILE_LINE;

  // Но еще один attach() без next() все еще запрещен
  tmp = std::make_unique<data_type>(3);
  att = buf.attach(std::move(tmp));
  t << is_true<assert>( att != nullptr) << FAS_TESTING_FILE_LINE;
  
  nxt1.second = 2;
  nxt1.first -= 1;
  bool res = buf.confirm(nxt1);
  t << is_false<assert>( res ) << FAS_TESTING_FILE_LINE;
  nxt1.first += 2;
  res = buf.confirm(nxt1);
  t << is_false<assert>( res ) << FAS_TESTING_FILE_LINE;
  nxt1.first -= 1;
  nxt1.second = 4;
  res = buf.confirm(nxt1);
  t << is_false<assert>( res ) << FAS_TESTING_FILE_LINE;
  nxt1.second = 1;
  res = buf.confirm(nxt1);
  t << is_true<assert>( res ) << FAS_TESTING_FILE_LINE;
}

UNIT(basic_sep1, "")
{
  using namespace fas::testing;
  typedef read_buffer::options_type options_type;
  std::shared_ptr<options_type> opt = std::make_shared<options_type>();
  opt->sep="\n";
  read_buffer buf;
  buf.set_options(opt);
  
  std::vector< std::string > result;
  size_t arr1[] = {3, 1, 3, 4, 3, 6};
  char *str1 = "aa\n\nbb\n\ncc\n\ndd\n\nee\n\n";
  const char str2[][10]={"aa\n", "\n", "bb\n", "\ncc\n", "\ndd", "\n\nee\n\n"};
  
  // настроено один next на один attach
  for ( size_t i = 0 ; i < 6; ++i )
  {
    t << message("i=") << i;
    if ( buf.need_buffer() )
    {
      auto res = buf.attach( std::make_unique<data_type>(arr1[i]) );
      t << is_true<assert>( res == nullptr ) << FAS_TESTING_FILE_LINE;
    }
    
    auto p = buf.next();
    t << is_true<assert>( p.first != nullptr ) << FAS_TESTING_FILE_LINE;
    t << is_true<assert>( p.second == arr1[i] ) << p.second << "!=" << arr1[i] << FAS_TESTING_FILE_LINE;
    t << stop;
    std::memcpy(p.first, str2[i], p.second );
    bool cfrm = buf.confirm(p);
    t << is_true<assert>( cfrm ) << FAS_TESTING_FILE_LINE;
    auto d = buf.detach();
    t << is_true<assert>( d != nullptr ) << FAS_TESTING_FILE_LINE;
    t << stop;
    while ( d!=nullptr )
    {
      result.push_back(std::string(d->begin(), d->end()));
      d = buf.detach();
      t << message("string=") << result.back() << ":" << result.back().size();
    }
  }
}


UNIT(basic_sep2, "")
{
  using namespace fas::testing;
  typedef read_buffer::options_type options_type;
  std::shared_ptr<options_type> opt = std::make_shared<options_type>();
  opt->sep="\r\n";
  read_buffer buf;
  buf.set_options(opt);
  
  std::vector< std::string > result;
  size_t arr1[] = {4, 1, 3, 8, 1};
  size_t arr2[] = {4, 1, 3, 8, 1};
  char *str1 = "aa\r\n\r\nb\r\ncc\r\ndd\r\n";
  const char str2[][10]={"aa\n\r", "\r", "\nb\r", "\ncc\r\ndd\r", "\n"};
  
  // настроено один next на один attach
  for ( size_t i = 0 ; i < 5; ++i )
  {
    t << message("i=") << i;
    if ( buf.need_buffer() )
    {
      auto res = buf.attach( std::make_unique<data_type>(arr1[i]) );
      t << is_true<assert>( res == nullptr ) << FAS_TESTING_FILE_LINE;
    }
    
    auto p = buf.next();
    t << is_true<assert>( p.first != nullptr ) << FAS_TESTING_FILE_LINE;
    t << is_true<assert>( p.second == arr1[i] ) << FAS_TESTING_FILE_LINE;
    t << stop;
    std::memcpy(p.first, str2[i], p.second );
    bool cfrm = buf.confirm(p);
    t << is_true<assert>( cfrm ) << FAS_TESTING_FILE_LINE;
    auto d = buf.detach();
    t << is_true<assert>( d != nullptr ) << FAS_TESTING_FILE_LINE;
    t << stop;
    while ( d!=nullptr )
    {
      result.push_back(std::string(d->begin(), d->end()));
      d = buf.detach();
    }
  }
}


BEGIN_SUITE(read_buffer, "read_buffer suite")
  ADD_UNIT(basic_test)
  ADD_UNIT(basic_sep1)
  //ADD_UNIT(basic_sep2)
END_SUITE(read_buffer)

BEGIN_TEST
  RUN_SUITE(read_buffer)
END_TEST
