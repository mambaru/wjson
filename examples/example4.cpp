#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <iostream>

int main()
{
  typedef wjson::value<int> int_json;
  typedef std::vector<int> vint_t;
  typedef wjson::array< std::vector<int_json> > vint_json;
  
  std::string json="[     1,\t2,\r3,\n4, /*пять*/ 5 ]";
  vint_t vint;
  vint_json::serializer()(vint, json.begin(), json.end(), NULL);
  json.clear();
  vint_json::serializer()(vint, std::back_inserter(json));
  std::cout << json << std::endl;
  
  typedef std::vector< vint_t > vvint_t;
  typedef wjson::array< std::vector<vint_json> > vvint_json;
  json="[ [], [1], [2, 3], [4, 5, 6] ]";
  vvint_t vvint;
  vvint_json::serializer()(vvint, json.begin(), json.end(), NULL);
  json.clear();
  vvint_json::serializer()(vvint, std::back_inserter(json));
  std::cout << json << std::endl;
  
  typedef std::vector< vvint_t > vvvint_t;
  typedef wjson::array< std::vector<vvint_json> > vvvint_json;
  json="[ [[]], [[1]], [[2], [3]], [[4], [5, 6] ] ]";
  vvvint_t vvvint;
  vvvint_json::serializer()(vvvint, json.begin(), json.end(), NULL);
  json.clear();
  vvvint_json::serializer()(vvvint, std::back_inserter(json));
  std::cout << json << std::endl;
  
  return 0;
}