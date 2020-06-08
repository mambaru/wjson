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

UNIT(parse_bool, "")
{
  using namespace fas::testing;
  using namespace wjson;
  std::string json="true";
  json_error er;
  std::string::iterator itr = parser::parse_bool(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
  parser::parse_bool(json.end(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
  er.reset();
  json.resize(3);
  parser::parse_bool(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="truE";
  er.reset();
  parser::parse_bool(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="falsE";
  er.reset();
  parser::parse_bool(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="False";
  er.reset();
  parser::parse_bool(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
}

UNIT(parse_integer, "")
{
  using namespace fas::testing;
  using namespace wjson;
  std::string json="-3e-12";
  json_error er;
  std::string::iterator itr = parser::parse_integer(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.begin() + 2 ) <<FAS_FL;
  parser::parse_integer(json.end(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
  er.reset();

  json.erase(json.begin(), json.begin()+2); // e-12
  parser::parse_integer(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="-";
  er.reset();
  parser::parse_integer(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="-123";
  er.reset();
  itr = parser::parse_integer(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;

  json="-123.X";
  er.reset();
  itr = parser::parse_integer(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.begin()+4) <<FAS_FL;

  json="-A";
  er.reset();
  parser::parse_integer(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="1.";
  er.reset();
  itr = parser::parse_integer(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.begin() + 1 ) <<FAS_FL;


  json="123eX";
  er.reset();
  itr = parser::parse_integer(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.begin() + 3 ) <<FAS_FL;
}

UNIT(parse_number, "")
{
  using namespace fas::testing;
  using namespace wjson;
  std::string json="-3e-12";
  json_error er;
  std::string::iterator itr = parser::parse_number(json.begin(), json.end(), &er);
  t << is_false<assert>(er) << FAS_FL;
  t << equal<assert>(itr, json.end() ) <<FAS_FL;
  parser::parse_number(json.end(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
  er.reset();
  json.resize(3);
  parser::parse_number(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="-";
  er.reset();
  parser::parse_number(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="-";
  er.reset();
  parser::parse_number(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="-A";
  er.reset();
  parser::parse_number(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="1.";
  er.reset();
  parser::parse_number(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="1eX";
  er.reset();
  parser::parse_number(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;
}

UNIT(parse_string, "")
{
  using namespace fas::testing;
  using namespace wjson;
  std::string json="";
  t << is_false<assert>(wjson::parser::is_string(json.begin(), json.end()) ) << FAS_FL;

  json="\"\\x\"";
  json_error er;
  parser::parse_string(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

  json="\"\\u123\"";
  er.reset();
  parser::parse_string(json.begin(), json.end(), &er);
  t << is_true<assert>(er) << FAS_FL;

}

} // namespace

BEGIN_SUITE(parser, "parser")
  ADD_UNIT(parse_space)
  ADD_UNIT(parse_null)
  ADD_UNIT(parse_bool)
  ADD_UNIT(parse_number)
  ADD_UNIT(parse_integer)
  ADD_UNIT(parse_string)
END_SUITE(parser)

