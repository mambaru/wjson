#include <fas/testing.hpp>
#include <wjson/parser.hpp>
#include <string>

namespace {

UNIT(parse_space, "")
{
  using namespace fas::testing;
  using namespace wjson;
  std::string json="\t\n/*Комментарий */  ";
  json_error er;
  std::string::iterator itr = parser::parse_space(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
  json="\t\n//*Комментарий *";
  itr = parser::parse_space(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
  
  bool is_space = parser::is_space(json.begin(), json.end());
  t << is_true<assert>(is_space) << FAS_FL;
  is_space = parser::is_space(json.begin()+2, json.end());
  t << is_false<assert>(is_space) << FAS_FL;
  is_space = parser::is_space(json.begin()+3, json.end());
  t << is_true<assert>(is_space) << FAS_FL;
  is_space = parser::is_space(json.begin()+4, json.end());
  t << is_false<assert>(is_space) << FAS_FL;
  is_space = parser::is_space(json.end(), json.end());
  t << is_false<assert>(is_space) << FAS_FL;
}

UNIT(parse_null, "")
{
  using namespace fas::testing;
  using namespace wjson;
  std::string json="null";
  json_error er;
  std::string::iterator itr = parser::parse_null(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
  json.resize(3);
  itr = parser::parse_null(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
  itr = parser::parse_null(json.end(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
  json="nulx";
  itr = parser::parse_null(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
  json="ull";
  itr = parser::parse_null(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
}

} // namespace

BEGIN_SUITE(parser, "parser")
  ADD_UNIT(parse_space)
  ADD_UNIT(parse_null)
END_SUITE(parser)

