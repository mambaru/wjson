
#include <iow/io/aux/read_buffer.hpp>
#include <fas/testing.hpp>
#include <cstring>
//#include <ucommon/socket.h>

typedef std::vector<char> data_type;
typedef std::string sep_type;
typedef ::iow::io::read_buffer<data_type> read_buffer;
typedef read_buffer::options_type options;

UNIT(basic_test, "")
{
  using namespace fas::testing;
  read_buffer buf;
  options opt;
  buf.get_options(opt);
  opt.bufsize = 3;
  buf.set_options(opt);
  
  //auto tmp = std::make_unique<data_type>(3);
  //! auto att = buf.attach(std::move(tmp));
   
  //t << is_true<assert>( att == nullptr) << FAS_TESTING_FILE_LINE;
  
  // Еще один attach() без next() запрещен
  //tmp = std::make_unique<data_type>(3);
  //! att = buf.attach(std::move(tmp));
  //t << is_true<assert>( att != nullptr) << FAS_TESTING_FILE_LINE;
  
  auto nxt1 = buf.next();
  t << is_true<assert>( nxt1.first != nullptr) << FAS_TESTING_FILE_LINE;
  t << is_true<assert>( nxt1.second == 3) << FAS_TESTING_FILE_LINE;
  std::strcpy(nxt1.first, "ab");
  // Еще один next() без confirm() запрещен
  auto nxt2 = buf.next();
  t << is_true<assert>( nxt2.first == nullptr) << FAS_TESTING_FILE_LINE;
  t << is_true<assert>( nxt2.second == 0) << FAS_TESTING_FILE_LINE;
  
  // После next() разрешен attach()
  //tmp = std::make_unique<data_type>(3);
  //! att = buf.attach(std::move(tmp));
  //t << is_true<assert>( att == nullptr) << FAS_TESTING_FILE_LINE;

  // Но еще один attach() без next() все еще запрещен
  //tmp = std::make_unique<data_type>(3);
  //! att = buf.attach(std::move(tmp));
  //t << is_true<assert>( att != nullptr) << FAS_TESTING_FILE_LINE;
  
  nxt1.second = 2;
  nxt1.first -= 1;
  bool res = buf.confirm(nxt1);
  t << is_false<assert>( res ) << FAS_TESTING_FILE_LINE;
  nxt1.first += 2;
  res = buf.confirm(nxt1);
  t << is_false<assert>( res ) << FAS_TESTING_FILE_LINE;
  nxt1.first -= 1;
  nxt1.second = 4096 + 1;
  res = buf.confirm(nxt1);
  t << is_false<assert>( res ) << FAS_TESTING_FILE_LINE;
  nxt1.second = 2;
  res = buf.confirm(nxt1);
  t << is_true<assert>( res ) << FAS_TESTING_FILE_LINE;
  
  auto d = buf.detach();
  t << is_true<assert>( d != nullptr) << FAS_TESTING_FILE_LINE;
  t << stop;
  t << message("size:") << d->size();
  auto str = std::string(d->begin(), d->end());
  t << message("string:") << str;
  t << equal_str<assert>( std::string("ab"), str  ) << FAS_TESTING_FILE_LINE;
}

template<typename T>
void test_buff1(T& t, read_buffer& buf, std::vector<std::string> reads, std::vector<std::string> chk)
{
  using namespace fas::testing;
  t << message("--test_buff1--");
  //std::vector<std::string> reads({args...});
  std::string incoming;
  std::string result;
  std::vector<std::string> vectres;
  for (auto& s: reads)
  {
    t << message("[") << s << "]";
    incoming+= s;
    auto p = buf.next();
    t << is_true<assert>(s.size() <= p.second) << FAS_TESTING_FILE_LINE;
    t << stop;
    std::strcpy( p.first, s.c_str());
    p.second = s.size();
    bool confirm = buf.confirm(p);
    t << is_true<assert>( confirm ) << FAS_TESTING_FILE_LINE;
    auto d = buf.detach();
    while ( d!=nullptr )
    {
      t << is_true<assert>( !d->empty() ) << FAS_TESTING_FILE_LINE;
      t << stop;
      vectres.push_back(std::string(d->begin(), d->end()));
      result += vectres.back();
      d = buf.detach();
    }
  }
  t << equal<expect>(incoming, result) << incoming << "!=" << result << FAS_TESTING_FILE_LINE;
  t << equal<expect>(chk, vectres) << FAS_TESTING_FILE_LINE;
}

template<typename T>
void test_buff2(T& t, read_buffer& buf, std::vector<std::string> reads, std::vector<std::string> chk)
{
  using namespace fas::testing;
  t << message("--test_buff2--");
  //std::vector<std::string> reads({args...});
  //reads.emplace_back( std::forward<std::string>(args)...);
  //=std::vector<std::string>( std::string(args)...); 
  std::string incoming;
  std::string result;
  for (auto& s: reads)
  {
    incoming+= s;
    auto p = buf.next();
    t << is_true<assert>(s.size() <= p.second) << FAS_TESTING_FILE_LINE;
    t << stop;
    std::strcpy( p.first, s.c_str());
    p.second = s.size();
    bool confirm = buf.confirm(p);
    t << is_true<assert>( confirm ) << FAS_TESTING_FILE_LINE;
  }
  
  std::vector<std::string> vectres;
  size_t count = 0;
  auto d = buf.detach();
  while ( d!=nullptr )
  {
    ++count;
    t << is_true<assert>( !d->empty() ) << FAS_TESTING_FILE_LINE;
    t << stop;
    auto str = std::string(d->begin(), d->end());
    vectres.push_back(str);
    result += str;
    d = buf.detach();
  }

  t << message("result:") << result;
  t << message("count:") << count;
  t << equal<expect>(incoming, result) << incoming << "!=" << result << FAS_TESTING_FILE_LINE;
  t << equal<expect>(chk, vectres) << FAS_TESTING_FILE_LINE;
  
}


template<typename T/*, typename ...Args*/>
void test_buff(T& t, read_buffer& buf, /*Args&&... args*/ std::vector<std::string> reads, std::vector<std::string> chk)
{
  test_buff1(t, buf, reads, chk);
  test_buff2(t, buf, reads, chk);
}




UNIT(basic_sep1, "")
{
  using namespace fas::testing;
  read_buffer buf;
  options opt;
  buf.get_options(opt);
  opt.sep="|";
  opt.bufsize = 6;
  opt.minbuf = 3;
  buf.set_options(opt);
  test_buff(t, buf, 
            {"aa|", "|", "bb|", "|cc|", "|dd", "||ee||"}, 
            {"aa|", "|", "bb|", "|", "cc|", "|", "dd|", "|", "ee|", "|"}
           );
  opt.bufsize = 1;
  buf.set_options(opt);
  test_buff(t, buf, 
            {"a", "a", "|", "|", "b", "b", "|", "|", "c", "c", "|", "|", "d", "d", "|", "|", "e", "e", "|", "|"},
            {"aa|", "|", "bb|", "|", "cc|", "|", "dd|", "|", "ee|", "|"}
           );
  
  opt.bufsize = 2;
  buf.set_options(opt);
  test_buff(t, buf, 
            {"aa", "||", "bb", "||", "cc", "||", "dd", "||", "ee", "||"},
            {"aa|", "|", "bb|", "|", "cc|", "|", "dd|", "|", "ee|", "|"}
           );

  /*
  options opt;
  opt.sep="\n";
  read_buffer buf;
  buf.get_options(opt);
  opt.bufsize = 1;
  buf.set_options(opt);
  
  std::string result;
  size_t arr1[] = {3, 1, 3, 4, 3, 6};
  const char *str1 = "aa\n\nbb\n\ncc\n\ndd\n\nee\n\n";
  const char str2[][10]={"aa\n", "\n", "bb\n", "\ncc\n", "\ndd", "\n\nee\n\n"};
  
  // настроено один next на один attach
  for ( size_t i = 0 ; i < 6; ++i )
  {
    t << message("i=") << i;
    //if ( buf.need_buffer() )
    {
      //auto res = buf.attach( std::make_unique<data_type>(arr1[i]) );
      //t << is_true<assert>( res == nullptr ) << FAS_TESTING_FILE_LINE;
    }
    
    auto p = buf.next();
    t << is_true<assert>( p.first != nullptr ) << FAS_TESTING_FILE_LINE;
    //! t << is_true<assert>( p.second == arr1[i] ) << p.second << "!=" << arr1[i] << FAS_TESTING_FILE_LINE;
    t << stop;
    std::memcpy(p.first, str2[i], p.second );
    bool cfrm = buf.confirm(p);
    t << is_true<assert>( cfrm ) << FAS_TESTING_FILE_LINE;
    auto d = buf.detach();
    t << is_true<assert>( d != nullptr ) << FAS_TESTING_FILE_LINE;
    t << stop;
    while ( d!=nullptr )
    {
      auto curres = std::string(d->begin(), d->end());
      result += curres;
      t << message("string=") << "[" << curres << "]" ;
      d = buf.detach();
    }
  }
  t << equal<assert, std::string>( result , std::string(str1) ) << "["<< result << "]" << FAS_TESTING_FILE_LINE;
  */
}


UNIT(basic_sep2, "")
{
  using namespace fas::testing;
}


BEGIN_SUITE(read_buffer, "read_buffer suite")
  ADD_UNIT(basic_test)
  ADD_UNIT(basic_sep1)
  //ADD_UNIT(basic_sep2)
END_SUITE(read_buffer)

BEGIN_TEST
  RUN_SUITE(read_buffer)
END_TEST
