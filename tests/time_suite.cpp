#include <fas/testing.hpp>
#include <wjson/wjson.hpp>

namespace {

struct A
{
  time_t timeout_s;
  time_t timeout_ms;
  time_t timeout_mks;
};

JSON_NAME(timeout_s)
JSON_NAME(timeout_ms)
JSON_NAME(timeout_mks)

typedef wjson::object<
  A,
  wjson::member_list<
    wjson::member<n_timeout_s, A, time_t, &A::timeout_s, wjson::time_interval<time_t, 1> >,
    wjson::member<n_timeout_ms, A, time_t, &A::timeout_ms, wjson::time_interval<time_t, 1000> >,
    wjson::member<n_timeout_mks, A, time_t, &A::timeout_mks, wjson::time_interval<time_t, 1000000> >
  >
> A_json;

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

  static const time_t ts = 3600*3 + 44;
  static const time_t tms =  ts * 1000 + 666;
  static const time_t tmks =  tms * 1000;
  std::string org = "{\"timeout_s\":\"3h44s\",\"timeout_ms\":\"3h44s666ms\",\"timeout_mks\":\"3h44s666000ms\"}";

  A a={ts, tms, tmks};
  json.clear();
  A_json::serializer()(a, std::back_inserter(json) );
  t << equal<expect>(json, org) << FAS_FL;

  A b={0, 0, 0};
  A_json::serializer()(b, json.begin(), json.end(), &e );
  t << is_false<expect>(e) << FAS_FL;
  t << equal<expect, time_t>(b.timeout_s, ts) << FAS_FL;
  t << equal<expect, time_t>(b.timeout_ms, tms) << FAS_FL;
  t << equal<expect, time_t>(b.timeout_mks, tmks) << FAS_FL;

  std::stringstream ss;
  ss << "{\"timeout_s\":" << ts
     << ",\"timeout_ms\":" << tms
     << ",\"timeout_mks\":" << tmks
     << "}";

  json = ss.str();
  A c={0, 0, 0};
  A_json::serializer()(c, json.begin(), json.end(), &e );
  t << is_false<expect>(e) << FAS_FL;
  t << equal<expect, time_t>(c.timeout_s, ts) << FAS_FL;
  t << equal<expect, time_t>(c.timeout_ms, tms) << FAS_FL;
  t << equal<expect, time_t>(c.timeout_mks, tmks) << FAS_FL;

}

}

BEGIN_SUITE(time_interval, "")
  ADD_UNIT(time_interval1)
END_SUITE(time_interval)
