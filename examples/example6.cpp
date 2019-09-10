#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <fas/system/nullptr.hpp>
#include <iostream>

int main()
{
  typedef wjson::value<int> int_json;
  typedef int vint_t[3];
  typedef wjson::array< int_json[3] > vint_json;

  std::string json="[     1,\t2,\n3,\n4,  5 ]";
  std::cout << json << " -> ";
  vint_t vint={100, 200, 300};
  typedef vint_json::serializer vint_ser;
  vint_ser()(vint, json.begin(), json.end(), fas_nullptr);
  json.clear();
  vint_ser()(vint, std::back_inserter(json));
  std::cout << json << std::endl << std::endl;

  typedef vint_t vvint_t[3];
  typedef wjson::array< vint_json[3] > vvint_json;
  json="[ [], [1], [2, 3], [4, 5, 6] ]";
  std::cout << json << " -> ";
  vvint_t vvint1={{100,100,100},{200, 200, 200}, {300, 300, 300}};
  vvint_json::serializer()(vvint1, json.begin(), json.end(), fas_nullptr);
  json.clear();
  vvint_json::serializer()(vvint1, std::back_inserter(json));
  std::cout << json << std::endl << std::endl;


  typedef vvint_t vvvint_t[3];
  typedef wjson::array< vvint_json[3] > vvvint_json;
  json="[ [[]], [[1]], [[2], [3]], [[4], [5, 6] ] ]";
  std::cout << json << " -> ";
  vvvint_t vvvint ={
    {
      {100, 101, 102},
      {110, 111, 112},
      {120, 121, 122}
    },
    {
      {200, 201, 202},
      {210, 211, 212},
      {220, 221, 222}
    },
    {
      {300, 301, 302},
      {310, 311, 312},
      {320, 321, 322}
    }
  };
  vvvint_json::serializer()(vvvint, json.begin(), json.end(), fas_nullptr);
  json.clear();
  vvvint_json::serializer()(vvvint, std::back_inserter(json));
  std::cout << json << std::endl;

  return 0;
}
