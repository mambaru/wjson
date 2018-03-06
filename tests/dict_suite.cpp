#include <fas/testing.hpp>
#include <wjson/json.hpp>
#include <wjson/name.hpp>



UNIT(dict1, "")
{
  using namespace fas::testing;
  using namespace wjson;

  {
    std::map<std::string, std::string> m;
    dict< std::map< value<std::string>, value<std::string> > >::serializer ser;
    m["раз"]="раз";
    m["два"]="два";
    std::string json;
    ser(m, std::back_inserter(json) );
    t << equal<expect>(json, "{\"два\":\"два\",\"раз\":\"раз\"}" ) << FAS_FL;
    m.clear();
    ser(m, json.begin(), json.end(), 0);
    t << equal<expect>(m.size(), 2ul ) << FAS_FL;
    t << equal<expect>(m.count("раз"), 1ul ) << FAS_FL;
    t << equal<expect>(m.count("два"), 1ul ) << FAS_FL;
    t << equal<expect>(m["раз"], "раз" ) << FAS_FL;
    t << equal<expect>(m["два"], "два" ) << FAS_FL;
  }

#if __cplusplus >= 201103L
  {
    std::unordered_map<std::string, std::string> m;
    dict< std::unordered_map< value<std::string>, value<std::string> > >::serializer ser;
    m["раз"]="раз";
    m["два"]="два";
    std::string json;
    ser(m, std::back_inserter(json) );
    t << equal<expect>(json, "{\"два\":\"два\",\"раз\":\"раз\"}" ) << FAS_FL;
    m.clear();
    ser(m, json.begin(), json.end(), 0);
    t << equal<expect>(m.size(), 2ul ) << FAS_FL;
    t << equal<expect>(m.count("раз"), 1ul ) << FAS_FL;
    t << equal<expect>(m.count("два"), 1ul ) << FAS_FL;
    t << equal<expect>(m["раз"], "раз" ) << FAS_FL;
    t << equal<expect>(m["два"], "два" ) << FAS_FL;
  }
#endif

  {
    std::vector< std::pair<std::string, std::string> > m;
    dict< std::vector< field< value<std::string>, value<std::string> > > >::serializer ser;
    m.push_back( std::make_pair("раз","раз") );
    m.push_back( std::make_pair("два","два") );
    std::string json;
    ser(m, std::back_inserter(json) );
    t << equal<expect>(json, "{\"раз\":\"раз\",\"два\":\"два\"}" ) << FAS_FL;
    m.clear();
    ser(m, json.begin(), json.end(), 0);
    t << equal<expect>(m.size(), 2ul ) << FAS_FL;
    t << equal<expect>(m[0].first, "раз" ) << FAS_FL;
    t << equal<expect>(m[0].second, "раз" ) << FAS_FL;
    t << equal<expect>(m[1].first, "два" ) << FAS_FL;
    t << equal<expect>(m[1].second, "два" ) << FAS_FL;
  }
}

UNIT(dict2, "")
{
  using namespace fas::testing;
  using namespace wjson;
  typedef dict_vector< value<std::string> > dv_t;
  typedef dv_t::target dict;
  dict d;
  dv_t::serializer ser;
  std::string json;
  ser(d, std::back_inserter(json) );
  t << equal<expect>(json, "{}" ) << FAS_FL;
  t << nothing;
}

BEGIN_SUITE(dict, "")
  ADD_UNIT(dict1)
  ADD_UNIT(dict2)
END_SUITE(dict)

