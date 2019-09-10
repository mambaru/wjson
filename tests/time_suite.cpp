#include <fas/testing.hpp>
#include <wjson/wjson.hpp>

UNIT(time_interval1, "")
{
  using namespace fas::testing;
  std::string json;
  time_t now = 1568112168666 + 3600000;
  wjson::time_interval<>::serializer()(now, std::back_inserter(json));
  t << equal<expect, std::string>(json, "\"18149d11h42m48s666ms\"") << FAS_FL;

  json.clear();
  now = 999;
  wjson::time_interval<>::serializer()(now, std::back_inserter(json));
  t << equal<expect, std::string>(json, "\"999ms\"") << FAS_FL;

  json.clear();
  now = 1000;
  wjson::time_interval<>::serializer()(now, std::back_inserter(json));
  t << equal<expect, std::string>(json, "\"1s\"") << FAS_FL;

  json.clear();
  now = 1001;
  wjson::time_interval<>::serializer()(now, std::back_inserter(json));
  t << equal<expect, std::string>(json, "\"1s1ms\"") << FAS_FL;

  json.clear();
  now = 61001;
  wjson::time_interval<>::serializer()(now, std::back_inserter(json));
  t << equal<expect, std::string>(json, "\"1m1s1ms\"") << FAS_FL;

  json.clear();
  now = 3661*1000 + 1 ;
  wjson::time_interval<>::serializer()(now, std::back_inserter(json));
  t << equal<expect, std::string>(json, "\"1h1m1s1ms\"") << FAS_FL;

  json.clear();
  now = (3600*24 + 3600 + 60 + 1)*1000 + 1;
  wjson::time_interval<>::serializer()(now, std::back_inserter(json));
  t << equal<expect, std::string>(json, "\"1d1h1m1s1ms\"") << FAS_FL;

  now=0;
  json="\"1d1h1m1s1ms\"";
  wjson::json_error e;
  wjson::time_interval<>::serializer()(now, json.begin(), json.end(), &e);
  t << is_false<expect>(e) << FAS_FL;
  t << equal<expect, time_t>(now, (3600*24 + 3600 + 60 + 1)*1000 + 1) << FAS_FL;

}

BEGIN_SUITE(time_interval, "")
  ADD_UNIT(time_interval1)
END_SUITE(time_interval)
